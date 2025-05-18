/*
 * node.c - skeleton vpp engine plug-in dual-loop node skeleton
 *
 * Copyright (c) <current-year> <your-organization>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/pg/pg.h>
#include <vppinfra/error.h>
#include <myplugin/myplugin.h>

typedef struct 
{
  u32 next_index;
  u32 sw_if_index;
  u8 new_src_mac[6];
  u8 new_dst_mac[6];
} myplugin_trace_t;

#ifndef CLIB_MARCH_VARIANT
static u8 *
my_format_mac_address (u8 * s, va_list * args)
{
  u8 *a = va_arg (*args, u8 *);
  return format (s, "%02x:%02x:%02x:%02x:%02x:%02x",
		 a[0], a[1], a[2], a[3], a[4], a[5]);
}

/* packet trace format function */
static u8 * format_myplugin_trace (u8 * s, va_list * args)
{
  CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
  myplugin_trace_t * t = va_arg (*args, myplugin_trace_t *);
  
  s = format (s, "MYPLUGIN: sw_if_index %d, next index %d\n",
              t->sw_if_index, t->next_index);
  s = format (s, "  new src %U -> new dst %U",
              my_format_mac_address, t->new_src_mac, 
              my_format_mac_address, t->new_dst_mac);
  return s;
}

vlib_node_registration_t myplugin_node;

#endif /* CLIB_MARCH_VARIANT */

#define foreach_myplugin_error \
_(SWAPPED, "Mac swap packets processed")

typedef enum {
#define _(sym,str) MYPLUGIN_ERROR_##sym,
  foreach_myplugin_error
#undef _
  MYPLUGIN_N_ERROR,
} myplugin_error_t;

#ifndef CLIB_MARCH_VARIANT
static char * myplugin_error_strings[] = 
{
#define _(sym,string) string,
  foreach_myplugin_error
#undef _
};
#endif /* CLIB_MARCH_VARIANT */

typedef enum 
{
  MYPLUGIN_NEXT_INTERFACE_OUTPUT,
  MYPLUGIN_N_NEXT,
} myplugin_next_t;

#define foreach_mac_address_offset              \
_(0)                                            \
_(1)                                            \
_(2)                                            \
_(3)                                            \
_(4)                                            \
_(5)



static inline void
ip4_decrement_ttl (ip4_header_t * ip)
{
  i32 ttl = ip->ttl;

  /* Input node should have reject packets with ttl 0. */
  ASSERT (ip->ttl > 0);

  u32 checksum = ip->checksum + clib_host_to_net_u16 (0x0100);
  checksum += checksum >= 0xffff;
  ip->checksum = checksum;
  ttl -= 1;
  ip->ttl = ttl;

  /* Verify checksum. */
  ASSERT (ip4_header_checksum_is_valid (ip));
}
static uword
ttl_handler (vlib_main_t * vm, vlib_node_runtime_t * node, vlib_frame_t * frame)
{
  u32 n_left_from, *from, next_index, *to_next, n_left_to_next;
  vlib_node_runtime_t *error_node =
    vlib_node_get_runtime (vm, myplugin_node.index);
  from = vlib_frame_vector_args (frame);
  n_left_from = frame->n_vectors;
  next_index = node->cached_next_index;
  
  clib_thread_index_t thread_index = vm->thread_index;
  u32 *buffer0 = 0;

  while (n_left_from > 0)
    {
      vlib_get_next_frame (vm, node, next_index, to_next, n_left_to_next);
      while (n_left_from > 0 && n_left_to_next > 0)
	{
	  u32 pi0;
	  vlib_buffer_t *p0;
	  ip4_header_t *ip40;
	  u16 port0 = 0;
	  ip6_header_t *ip6h0;
	  u32 next0 = MYPLUGIN_NEXT_INTERFACE_OUTPUT;
	  u32 domain_index0 = ~0;
	  bool free_original_buffer0 = false;
	  u32 *frag_from0, frag_left0;

	  pi0 = to_next[0] = from[0];
	  from += 1;
	  n_left_from -= 1;

	  p0 = vlib_get_buffer (vm, pi0);
	  ip40 = vlib_buffer_get_current (p0);

	

	 
	  /*
	   * Trial Can we get TCP header as well
	   */
	  if (ip40->protocol == IP_PROTOCOL_TCP)
	  {
	      tcp_header_t *tcp = ip4_next_header (ip40);
	  }

	  /* Decrement IPv4 TTL */
	  ip4_decrement_ttl (ip40);

    
	  bool df0 =
	    ip40->flags_and_fragment_offset &
	    clib_host_to_net_u16 (IP4_HEADER_FLAG_DONT_FRAGMENT);


	  /*
	   * Determine next node. Can be one of:
	   * ipv4-lookup, ip4-rewrite, error-drop
     * .
     * .
     * .
     * but we hardcoded to interface-output
	   */

	  if (PREDICT_FALSE (p0->flags & VLIB_BUFFER_IS_TRACED))
	    {
	      //map_add_trace (vm, node, p0, map_domain_index0, port0);
        /*
           Not sure , but guessing its for vpp trace of buffer, 
           not sure whether used following api correctly by me 
          */
        vlib_add_trace(vm, node, p0, sizeof(unsigned));
	    }

	  //p0->error = error_node->errors[error0];
    //TIll now packet has processed , now just forward the packets to next nodes
	//exit:
	  /* Send fragments that were added in the frame */
	  if (free_original_buffer0)
	    {
	      vlib_buffer_free_one (vm, pi0);	/* Free original packet */
	    }
	  else
	    {
	      vec_add1 (buffer0, pi0);
	    }

	  frag_from0 = buffer0;
	  frag_left0 = vec_len (buffer0);

	  while (frag_left0 > 0)
	    {
	      while (frag_left0 > 0 && n_left_to_next > 0)
		{
		  u32 i0;
		  i0 = to_next[0] = frag_from0[0];
		  frag_from0 += 1;
		  frag_left0 -= 1;
		  to_next += 1;
		  n_left_to_next -= 1;

		  vlib_validate_buffer_enqueue_x1 (vm, node, next_index,
						   to_next, n_left_to_next,
						   i0, next0);
		}
	      vlib_put_next_frame (vm, node, next_index, n_left_to_next);
	      vlib_get_next_frame (vm, node, next_index, to_next,
				   n_left_to_next);
	    }
	  vec_reset_length (buffer0);
	}
      vlib_put_next_frame (vm, node, next_index, n_left_to_next);
    }

  vec_free (buffer0);
  return frame->n_vectors;
}

/* *INDENT-OFF* */
#ifndef CLIB_MARCH_VARIANT
VLIB_REGISTER_NODE (myplugin_node) = 
{
  .function = ttl_handler, 
  .name = "myplugin",
  .vector_size = sizeof (u32),
  .format_trace = format_myplugin_trace,
  .type = VLIB_NODE_TYPE_INTERNAL,
  
  .n_errors = ARRAY_LEN(myplugin_error_strings),
  .error_strings = myplugin_error_strings,

  .n_next_nodes = MYPLUGIN_N_NEXT,

  /* edit / add dispositions here */
  .next_nodes = {
        [MYPLUGIN_NEXT_INTERFACE_OUTPUT] = "interface-output",
  },
};
#endif /* CLIB_MARCH_VARIANT */
/* *INDENT-ON* */
/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
