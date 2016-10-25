/**
 * The source code modifies the ns3's dsdv manet example,
 * which can be found under 'src/dsdv/examples/dsdv-manet.cc'
 **/
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "scratch/disable-nodes-updating-dsdv-experiment2/dsdv-helper.h"
#include <iostream>
#include <cmath>
#include "ns3/flow-monitor-module.h"
using namespace ns3;

uint16_t port = 9;
bool isVerbose = false;
NS_LOG_COMPONENT_DEFINE ("PartialNonUpdatingDsdvExperiment");

class DsdvManetExperiment
{
public:
  DsdvManetExperiment ();
  void CaseRun (uint32_t nWifis,
                uint32_t nSinks,
                double totalTime,
                std::string rate,
                std::string phyMode,
                uint32_t nodeSpeed,
                uint32_t periodicUpdateInterval,
                uint32_t settlingTime,
                double dataStart,
                bool printRoutes,
                std::string statsFileName, uint32_t silentNodes);

private:
  uint32_t m_nWifis;
  uint32_t m_nSinks;
  double m_totalTime;
  std::string m_rate;
  std::string m_phyMode;
  uint32_t m_nodeSpeed;
  uint32_t m_periodicUpdateInterval;
  uint32_t m_settlingTime;
  double m_dataStart;
  uint32_t bytesTotal;
  uint32_t packetsReceived;
  bool m_printRoutes;
  std::string m_statsFileName;
  // Added number of silent nodes
  uint32_t m_nSilentNodes;

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

private:
  void CreateNodes ();
  void CreateDevices (std::string tr_name);
  void InstallInternetStack (std::string tr_name);
  void InstallApplications ();
  void SetupMobility ();
  void ReceivePacket (Ptr <Socket> );
  Ptr <Socket> SetupPacketReceive (Ipv4Address, Ptr <Node> );
  void CheckThroughput ();

};

int main (int argc, char **argv)
{
  DsdvManetExperiment test;
  uint32_t nWifis = 50;
  uint32_t cbrNodes = 10;
  double totalTime = 100.0;
  std::string rate ("1Mbps");
  std::string phyMode ("DsssRate11Mbps");
  uint32_t nodeSpeed = 10; // in m/s
  std::string appl = "all";
  uint32_t periodicUpdateInterval = 15;
  uint32_t settlingTime = 6;
  double dataStart = 50.0;
  bool printRoutingTable = true;
  std::string statsFileName = "SkeletonDsdvExperiment-unmodified.stat";

  uint32_t silentNodes = 0;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Print trace and pcaps[Default:false]",isVerbose);
  cmd.AddValue ("nWifis", "Number of wifi nodes[Default:50]", nWifis);
  cmd.AddValue ("cbrNodes", "Number of wifi flows [Default:10]", cbrNodes);
  cmd.AddValue ("silentNodes", "Number of non-updating nodes [Default:0]", silentNodes);
  cmd.AddValue ("totalTime", "Total Simulation time[Default:100]", totalTime);
  cmd.AddValue ("phyMode", "Wifi Phy mode[Default:DsssRate11Mbps]", phyMode);
  cmd.AddValue ("rate", "CBR traffic rate[Default:1Mbps]", rate);
  cmd.AddValue ("nodeSpeed", "Node speed in RandomWayPoint model[Default:10]", nodeSpeed);
  cmd.AddValue ("periodicUpdateInterval", "Periodic Interval Time[Default=15]", periodicUpdateInterval);
  cmd.AddValue ("settlingTime", "Settling Time before sending out an update for changed metric[Default=6]", settlingTime);
  cmd.AddValue ("dataStart", "Time at which nodes start to transmit data[Default=50.0]", dataStart);
  cmd.AddValue ("printRoutingTable", "print routing table for nodes[Default:1]", printRoutingTable);
  cmd.AddValue ("statsFileName", "The name of the CSV output file name[Default:DsdvExperiment1-unmodified.stat]", statsFileName);
  cmd.Parse (argc, argv);


  SeedManager::SetSeed (12345);

  Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue ("1000"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (rate));
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2000"));

  test = DsdvManetExperiment ();
  test.CaseRun (nWifis, cbrNodes, totalTime, rate, phyMode, nodeSpeed, periodicUpdateInterval,
                settlingTime, dataStart, printRoutingTable, statsFileName, silentNodes);

  return 0;
}

DsdvManetExperiment::DsdvManetExperiment ()
  : bytesTotal (0),
    packetsReceived (0)
{
}

void
DsdvManetExperiment::ReceivePacket (Ptr <Socket> socket)
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " Received one packet!");
  Ptr <Packet> packet;
  while ((packet = socket->Recv ()))
    {
      bytesTotal += packet->GetSize ();
      packetsReceived += 1;
    }
}

void
DsdvManetExperiment::CheckThroughput ()
{
  double kbs = (bytesTotal * 8.0) / 1000;
  bytesTotal = 0;
  NS_LOG_UNCOND("Time: " << (Simulator::Now()).GetSeconds());
  if (isVerbose){
    std::ofstream out (m_statsFileName.c_str (), std::ios::app);

    out << (Simulator::Now ()).GetSeconds () << "," << kbs << "," << packetsReceived << "," << m_nSinks << std::endl;

    out.close ();
  }
  packetsReceived = 0;
  Simulator::Schedule (Seconds (1.0), &DsdvManetExperiment::CheckThroughput, this);
}

Ptr <Socket>
DsdvManetExperiment::SetupPacketReceive (Ipv4Address addr, Ptr <Node> node)
{

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr <Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback ( &DsdvManetExperiment::ReceivePacket, this));

  return sink;
}

void
DsdvManetExperiment::CaseRun (uint32_t nWifis, uint32_t nSinks, double totalTime, std::string rate,
                           std::string phyMode, uint32_t nodeSpeed, uint32_t periodicUpdateInterval, uint32_t settlingTime,
                           double dataStart, bool printRoutes, std::string statsFileName, uint32_t silentNodes)
{
  m_nWifis = nWifis;
  m_nSinks = nSinks;
  m_totalTime = totalTime;
  m_rate = rate;
  m_phyMode = phyMode;
  m_nodeSpeed = nodeSpeed;
  m_periodicUpdateInterval = periodicUpdateInterval;
  m_settlingTime = settlingTime;
  m_dataStart = dataStart;
  printRoutes = printRoutes;
  m_statsFileName = statsFileName;
  m_nSilentNodes = silentNodes;

  std::stringstream ss;
  ss << m_nWifis;
  std::string t_nodes = ss.str ();

  std::stringstream ss3;
  ss3 << m_totalTime;
  std::string sTotalTime = ss3.str ();

  std::string tr_name = "Dsdv_Manet_" + t_nodes + "Nodes_" + sTotalTime + "SimTime";
  if (isVerbose) std::cout << "Trace file generated is " << tr_name << ".tr\n";

  CreateNodes ();
  CreateDevices (tr_name);
  SetupMobility ();
  InstallInternetStack (tr_name);
  InstallApplications ();

  std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";

  CheckThroughput (); // Is this necessary?
  
  // Setup the flow monitor
  Ptr<FlowMonitor> monitor;
  FlowMonitorHelper flowHelper;
  monitor = flowHelper.InstallAll();

  Simulator::Stop (Seconds (m_totalTime));
  Simulator::Run ();

  monitor->CheckForLostPackets();

  int nRxPkts = 0;
  int nTxPkts = 0;
  // Flow monitor stats
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
  std::map<FlowId,FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
  for (std::map<FlowId,FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); ++iter)
  {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);

    NS_LOG_UNCOND("Flow "<< iter->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
    nTxPkts += iter->second.txPackets;
    nRxPkts += iter->second.rxPackets;
    NS_LOG_UNCOND("Tx Packets:\t" << iter->second.txPackets);
    NS_LOG_UNCOND("Rx Packets:\t" << iter->second.rxPackets);
    NS_LOG_UNCOND("Throughput " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024 / 1024 << " Mbps");
    }
  if (nTxPkts == 0) {nTxPkts = 1; nRxPkts = 0;} // stop divide by zero errors
  NS_LOG_UNCOND("Total received: " << nRxPkts << "/" << nTxPkts << " (" << (((float) nRxPkts / (float) nTxPkts)*100.0) << "%)");
  std::ofstream out (statsFileName.c_str ());
  out << "silentNodes,cbrNodes,nodeSpeed,throughput" << std::endl;
  out << silentNodes << "," << nSinks << "," << nodeSpeed << "," << ((float) nRxPkts / (float) nTxPkts) << std::endl;
  out.close ();
  Simulator::Destroy ();
}

void
DsdvManetExperiment::CreateNodes ()
{
  std::cout << "Creating " << (unsigned) m_nWifis << " nodes.\n";
  nodes.Create (m_nWifis);
  NS_ASSERT_MSG (m_nWifis > m_nSinks, "Sinks must be less or equal to the number of nodes in network");
}

void
DsdvManetExperiment::SetupMobility ()
{
  MobilityHelper mobility;
  ObjectFactory pos;
  pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"));

  std::ostringstream speedConstantRandomVariableStream;
  speedConstantRandomVariableStream << "ns3::ConstantRandomVariable[Constant="
                                   << m_nodeSpeed
                                   << "]";

  Ptr <PositionAllocator> taPositionAlloc = pos.Create ()->GetObject <PositionAllocator> ();
  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel", "Speed", StringValue (speedConstantRandomVariableStream.str ()),
                             "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=2.0]"), "PositionAllocator", PointerValue (taPositionAlloc));
  mobility.SetPositionAllocator (taPositionAlloc);
  mobility.Install (nodes);
}

void
DsdvManetExperiment::CreateDevices (std::string tr_name)
{
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (m_phyMode), "ControlMode",
                                StringValue (m_phyMode));
  devices = wifi.Install (wifiPhy, wifiMac, nodes);
  if (isVerbose){
    AsciiTraceHelper ascii;
    wifiPhy.EnableAsciiAll (ascii.CreateFileStream (tr_name + ".tr"));
    wifiPhy.EnablePcapAll (tr_name);
  }
}

void
DsdvManetExperiment::InstallInternetStack (std::string tr_name)
{
  // Configuration of Network Stack for nodes with updating
  DsdvHelper dsdv;
  dsdv.Set ("PeriodicUpdateInterval", TimeValue (Seconds (m_periodicUpdateInterval)));
  dsdv.Set ("SettlingTime", TimeValue (Seconds (m_settlingTime)));
  dsdv.Set("DisableUpdates",BooleanValue(true));

  InternetStackHelper stack;
  stack.SetRoutingHelper (dsdv); // has effect on the next Install () -- MODIFICATION: so we duplicated stack so it is not changed
 
  // Configuration of Network stack for nodes with no updating
  DsdvHelper dsdv2;
  dsdv2.Set ("PeriodicUpdateInterval", TimeValue (Seconds (m_periodicUpdateInterval)));
  dsdv2.Set ("SettlingTime", TimeValue (Seconds (m_settlingTime)));
  dsdv2.Set("DisableUpdates",BooleanValue(false));

  InternetStackHelper stack2;
  stack2.SetRoutingHelper (dsdv2); 


  Ipv4AddressHelper address;

  // We need to not bias the disabled nodes to be CBR traffic generating nodes
  std::vector<uint32_t> numVector;
  std::cout << m_nWifis << std::endl;
  for (uint32_t i = 0; i < m_nWifis; i++)
  {
    numVector.push_back(i);
  }
  std::random_shuffle(numVector.begin(),numVector.end());

  uint32_t nAllocatedSilentNodes = 0;
  for (uint32_t i = 0; i < m_nWifis; i++){
    int idx = numVector.at(i);
    if (nAllocatedSilentNodes < m_nSilentNodes)
    {
      std::cout << "Node #" << numVector.at(i) << " is not sending updates\n";
      stack.Install(nodes.Get(idx));
      nAllocatedSilentNodes++;
    }
    else 
    {
      stack2.Install(nodes.Get(idx)); 
    }
  }
  std::cout << std::endl;


  /** 
    * Preference for listeners of the CBR traffic to send ignore updates
    * - To prefer CBR sources to ignore updates, start at loop a m_nWifis - 1, and loop downwards
    * - This allows us to measure if ignore update activity is more important for generators of traffic
    *     and consumers of traffic
    **/

  /*
  // Based on the idea that sometimes nodes won't send updates to save on energy (the node is being greedy)...
  // We allow the disable update flag to be applied in same order as nodes that generate traffic.
  dsdv.Set("DisableUpdates",BooleanValue(true));
  stack.SetRoutingHelper (dsdv); // has effect on the next Install ()
  for (uint32_t i = 0; i < m_nSilentNodes; i++){
    stack.Install(nodes.Get(i));
  }
    for (uint32_t i = m_nSilentNodes; i < m_nWifis; i++){
    stack2.Install(nodes.Get(i));
  }
  */
  
  // for the first nSilentNodes
  // iterate over nodes and install stack to nSilentNodes nodes
  // for 
  //stack.Install (nodes);
 
  address.SetBase ("10.1.1.0", "255.255.255.0");
  interfaces = address.Assign (devices);
  if (m_printRoutes)
    {
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ((tr_name + ".routes"), std::ios::out);
      dsdv.PrintRoutingTableAllAt (Seconds (m_periodicUpdateInterval), routingStream);
    }
}

void
DsdvManetExperiment::InstallApplications ()
{
  uint32_t clientNode = m_nWifis - 1;
  for (uint32_t i = 0; i <= m_nSinks - 1; i++ )
    {
      // Setup CBR sinks
      Ptr<Node> sinkNode = NodeList::GetNode (i);
      Ipv4Address sinkNodeAddress = sinkNode->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();
      Ptr<Socket> sink = SetupPacketReceive (sinkNodeAddress, sinkNode);
      // Setup CBR source
      if (clientNode == i) clientNode--; // Ensures that flow does not use same source and sink node
      // The OnOff application generates traffic to a destination
      OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (interfaces.GetAddress (i), port)));
      onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
      onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
      ApplicationContainer apps1 = onoff1.Install (nodes.Get (clientNode));
      Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
      apps1.Start (Seconds (var->GetValue (m_dataStart, m_dataStart + 1)));
      apps1.Stop (Seconds (m_totalTime));
      clientNode--;
  }
}

