/* Define matching types for parameters on each side */
#ifdef HOST_INCLUSION
#define pval_uint_t uint32_t
#else
#define pval_uint_t uint
#endif

typedef struct {
  pval_uint_t global_dims[2];
  pval_uint_t global_row_stride;
  float xmin, xmax, ymin, ymax;
} params_t;

#ifndef HOST_INCLUSION
kernel void init_domain(constant params_t *params,
                        global float *output,
                        global float *diffs)
{
  const float PI = 4*atan(1);

  /* Compute the output position for this work item */
  uint opos = get_global_id(0) + params->global_row_stride*get_global_id(1);
  float x = params->xmin + (params->xmax - params->xmin)*
    convert_float(get_global_id(0))/convert_float(get_global_size(0));
  float y = params->ymin + (params->ymax - params->ymin)*get_global_id(1)/
    get_global_size(1);

  /* Handle boundary conditions and initialize the interior to zero */
  float oval = 0;
  oval = select(oval, sin(2*y), get_global_id(0) == 0);
  oval = select(oval, sin(y/2), get_global_id(0) == get_global_size(0) - 1);
  oval = select(oval, x/PI, get_global_id(1) == get_global_size(1) - 1);

  output[opos] = oval;

  /* Initialize the difference matrix, this mostly matters for the edges */
  diffs[opos] = 0;
}

kernel void jacobi_step(constant params_t *params,
                          global float *state,
                          global float *diffs,
                          local float *ltile)
{
  /* Local tile row stride */
  const uint lt_row_stride = get_local_size(0) + 2;
  /* Compute the location of our element within the local tile */
  const uint ltpos = 1 + get_local_id(0) + (1 + get_local_id(1))*lt_row_stride;

  /* Length of row to read from the global space */
  const uint row_read_len = get_local_size(0) +
    /* Add one if there's a column to the left */
    isgreater(get_group_id(0), 0) +
    /* Add one if there's a column to the right */
    isless(get_group_id(0), get_num_groups(0) - 1);

  /* Number of rows to read */
  const uint row_read_count = get_local_size(1) +
    /* Add one if there's a row beneath us */
    isgreater(get_group_id(1), 0) +
    /* Add one if there's a row above us */
    isless(get_group_id(1), get_num_groups(1) - 1);

  /* Position the read window. First compute the position of this work group's
   * lower left corner in the global array
   */
  const uint wgpos = get_group_id(0)*get_local_size(0) +
    get_group_id(1)*get_local_size(1)*params->global_row_stride;
  /* Compute the position of the window from which we read the local tile.
   */
  global float *global_pos = state + wgpos -
    /* Move one unit back if there's a column to the left we need to read */
    isgreater(get_group_id(0), 0) -
    /* And move down a row if we need to include that one */
    isgreater(get_group_id(1), 0)*params->global_row_stride;

  /* Location to start reading into the tile */
  local float *local_pos = ltile + isequal(get_group_id(0), 0) +
    isequal(get_group_id(1), 0)*lt_row_stride;

  /* Collective read of local tile */
  event_t copy_complete = 0;
  for(int r = 0; r < row_read_count; ++r) {
    copy_complete = async_work_group_copy(
      local_pos, global_pos, row_read_len, copy_complete);
    local_pos += lt_row_stride;
    global_pos += params->global_row_stride;
  }
  wait_group_events(1, &copy_complete);

  /* Determine if we're on the edge and use this to do nothing to preserve the
   * boundaries.
   */
  bool edge = get_global_id(0) == 0 ||
              get_global_id(0) == get_global_size(0) - 1 ||
              get_global_id(1) == 0 ||
              get_global_id(1) == get_global_size(1) - 1;

  /* Two different cases based on stability */
  const float dx = (params->xmax - params->xmin)/get_global_size(0);
  const float dy = (params->ymax - params->ymin)/get_global_size(1);
  const float lambda2 = pow(dx/dy,2)*isless(dx,dy) +
                        pow(dy/dx,2)*isgreaterequal(dx,dy);
  const uint idx1 = ltpos - lt_row_stride*isless(dx,dy) - isgreaterequal(dx,dy);
  const uint idx2 = ltpos + lt_row_stride*isless(dx,dy) + isgreaterequal(dx,dy);
  const uint idx3 = ltpos - isless(dx,dy) - lt_row_stride*isgreaterequal(dx,dy);
  const uint idx4 = ltpos + isless(dx,dy) + lt_row_stride*isgreaterequal(dx,dy);

  float updval = ltile[idx1] + ltile[idx2];
  updval *= lambda2;
  updval += ltile[idx3] + ltile[idx4];
  updval /= 2*(1 + lambda2);

  ltile[ltpos] = select(updval, ltile[ltpos], edge);

  /* Collective output of local tile */
  barrier(CLK_LOCAL_MEM_FENCE);
  copy_complete = 0;
  global_pos = state + wgpos;
  local_pos = ltile + 1 + lt_row_stride;
  for(int r = 0; r < get_local_size(1); ++r) {
    copy_complete = async_work_group_copy(
      global_pos, local_pos, get_local_size(0), copy_complete);
    local_pos += lt_row_stride;
    global_pos += params->global_row_stride;
  }
  wait_group_events(1, &copy_complete);
}
#endif /* HOST_INLCUSION */

// vim: filetype=c
