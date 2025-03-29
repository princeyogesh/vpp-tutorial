//
#include <iostream>
#include <algorithm>
#include <vapi/vapi.hpp>
#include <vapi/vpe.api.vapi.hpp>
DEFINE_VAPI_MSG_IDS_VPE_API_JSON
#include <vapi/interface.api.vapi.hpp>
DEFINE_VAPI_MSG_IDS_INTERFACE_API_JSON
#include <vapi/ip.api.vapi.hpp>
DEFINE_VAPI_MSG_IDS_IP_API_JSON
#include <vapi/ipip.api.vapi.hpp>
DEFINE_VAPI_MSG_IDS_IPIP_API_JSON
#include <vapi/ipsec.api.vapi.hpp>
//#include <vnet/ip/punt.api.h>
//#include <vapi/punt.api.vapi.hpp>
//#include <vpp/api/vpe.api.h>
DEFINE_VAPI_MSG_IDS_IPSEC_API_JSON

template <typename MyRequest>
static auto& execute (vapi::Connection &con, MyRequest &req)
{
  // send the command to VPP
  auto err = req.execute ();
  std::cout << " req.execute() = " << err << std::endl;
  if (VAPI_OK != err)
    throw std::runtime_error ("execute()");
  // active-wait for command result
  do
    {
      err = con.wait_for_response (req);
      std::cout << " err = " << err << std::endl;
    }
  while (VAPI_EAGAIN == err);
  if (VAPI_OK != err)
    throw std::runtime_error ("wait_for_response()");
  // verify the reply error code
  auto &rmp = req.get_response ().get_payload ();
  std::cerr << "rmp.retval1 " << rmp.retval << std::endl;
  if (0 != rmp.retval)
    throw std::runtime_error ("wrong return code");
  std::cerr << "rmp.retval" << rmp.retval << std::endl;
 // std::cout << "version is " << 
 // std:: cout << "vpp version is "<< rmp.version << std::endl; 
  return rmp;
}

int main()
{
  std::cout << "Connecting to VPP..." << std::endl;
  vapi::Connection con;
  auto err = con.connect ("example_client", nullptr, 32, 32);
  if (VAPI_OK != err)
    throw std::runtime_error ("connection to VPP failed");

  try
  {
    std::cout << "Showing Version..." << std::endl;
    vapi::Show_version req(con);
    //auto& mp = req.get_request();

    auto resp = execute (con, req); 
    std::cout << "vpp version of " << resp.version << std::endl;
    //create interface with vpp & assign ip
    vapi::Sw_interface_add_del_address setipaddr_req(con);
    const unsigned char IP[]  = { 192, 168, 1, 8 };
    auto& ip = setipaddr_req.get_request().get_payload();
    ip.is_add = true;
    ip.sw_if_index = 1;
    ip.prefix.address.af = ADDRESS_IP4;
    ip.prefix.len = 24;
    std::copy (IP, IP + 4, ip.prefix.address.un.ip4);
    auto ip_resp = execute(con, setipaddr_req);
    std::cout << " set IP address returned "<< ip_resp.retval << std::endl;
    
    //add route in vpp to route the packets to server
    vapi::Ip_route_add_del_v2 routereq(con, 1);
    const unsigned char prefix[]  = { 192, 168, 0, 0 };
    const unsigned char nh[] = {192, 168, 1, 9};  //nexthop address
    auto& mp = routereq.get_request().get_payload();
    mp.is_add = true;
    mp.is_multipath= true;
    // mp.route.table_id = 999; not needed 
    mp.route.prefix.address.af = ADDRESS_IP4;
    mp.route.prefix.len = 24;
    mp.route.n_paths = 1;
    mp.route.paths[0].sw_if_index = 1;
    mp.route.paths[0].proto = FIB_API_PATH_NH_PROTO_IP4;

    std::copy (prefix, prefix + 4, mp.route.prefix.address.un.ip4);
    std::copy (nh, nh + 4, mp.route.paths[0].nh.address.ip4);
    
    auto route_resp = execute(con, routereq);
    std::cout << "route returned " << route_resp.retval << std::endl;
    std::cout << " route returned " << route_resp.stats_index << std::endl;
    
  }
  catch (...)
  {
    std::cerr << "catching exceptions"  << std::endl;
  }
  con.disconnect ();
  std::cout << "Success" << std::endl;
    return 0;
}