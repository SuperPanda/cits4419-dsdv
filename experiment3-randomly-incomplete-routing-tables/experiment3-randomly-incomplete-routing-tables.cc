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
#include "ns3/internet-stack-helper.h"
#include "ns3/internet-module.h"
#include "scratch/experiment3-randomly-incomplete-routing-tables/dsdv-helper.h"
#include <iostream>
#include <cmath>
#include "ns3/flow-monitor-module.h"
using namespace ns3;

uint16_t port = 9;
bool isVerbose = false;
NS_LOG_COMPONENT_DEFINE ("DsdvManetExperiment");

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
                std::string statsFileName,
                double rtCorruptionProbability);

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

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

  double m_rtCorruptionProbability;

private:
  void CreateNodes ();
  void CreateDevices (std::string tr_name);
  void InstallInternetStack (std::string tr_name, std::vector<DsdvHelper>& dsdvHelper);
  void InstallApplications ();
  void SetupMobility ();
  void ReceivePacket (Ptr <Socket> );
  Ptr <Socket> SetupPacketReceive (Ipv4Address, Ptr <Node> );
  void CheckProgress ();

};

int main (int argc, char **argv)
{
  //LogComponentEnable("IgnoringDsdvRoutingProtocol", LOG_LEVEL_INFO);
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
  double rtCorruptionProbability = 0.0;
  bool printRoutingTable = true;
  std::string statsFileName = "SkeletonDsdvExperiment-unmodified.stat";

  CommandLine cmd;
  cmd.AddValue ("verbose", "Print trace and pcaps[Default:false]",isVerbose);
  cmd.AddValue ("rtCorruptionProbability", "Probability that the routing table will have a missing entry", rtCorruptionProbability);
  cmd.AddValue ("nWifis", "Number of wifi nodes[Default:50]", nWifis);
  cmd.AddValue ("cbrNodes", "Number of wifi flows [Default:10]", cbrNodes);
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
                settlingTime, dataStart, printRoutingTable, statsFileName, rtCorruptionProbability);

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

/**
 * Should be renamed to CheckProgress
 */
void
DsdvManetExperiment::CheckProgress ()
{
  double kbs = (bytesTotal * 8.0) / 1000;
  bytesTotal = 0;
  float percentComplete = (float) (100.0*Simulator::Now()).GetSeconds() / (float) m_totalTime;
  //int ticks = (int) percentComplete;
  std::cout << "\rProgress [";
  int sizeOfBar = 30;
  for (int i = 0; i < sizeOfBar; i++)
  {
    if ((float) i < (((float) sizeOfBar)*(float) percentComplete/100.0))
    {
      std::cout << "#";
    }
    else
    {
      std::cout << " ";
    }
  }
  std::cout << "] " << percentComplete << "%\t";
  std::cout << "\tSimulation time: " << (Simulator::Now()).GetSeconds() << "s / " << m_totalTime << "s\t\t";
  std::cout << std::flush;
  if (isVerbose){
    std::ofstream out (m_statsFileName.c_str (), std::ios::app);

    out << (Simulator::Now ()).GetSeconds () << "," << kbs << "," << packetsReceived << "," << m_nSinks << std::endl;

    out.close ();
  }
  packetsReceived = 0;
  Simulator::Schedule (Seconds (1.0), &DsdvManetExperiment::CheckProgress, this);
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
                           double dataStart, bool printRoutes, std::string statsFileName, double rtCorruptionProbability)
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
  m_printRoutes = printRoutes;
  m_statsFileName = statsFileName;
  m_rtCorruptionProbability = rtCorruptionProbability;
  std::vector<DsdvHelper> dsdvHelpers = std::vector<DsdvHelper>();

  std::stringstream ss;
  ss << m_nWifis;
  std::string t_nodes = ss.str ();

  std::stringstream ss3;
  ss3 << m_totalTime;
  std::string sTotalTime = ss3.str ();

   std::stringstream sstrace; 
   sstrace <<  "Trace-experiment3-cbrNodes" << m_nSinks <<"-nodeSpeed"<<nodeSpeed<<"-rtCorruptionProbability"<<rtCorruptionProbability;
  std::string tr_name = sstrace.str();
  std::cout << "Trace file generated is " << tr_name << ".tr\n";

  CreateNodes ();
  CreateDevices (tr_name);
  SetupMobility ();
  InstallInternetStack (tr_name,dsdvHelpers);
  InstallApplications ();

  std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";

  CheckProgress (); // Is this necessary?
  
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
    //NS_LOG_UNCOND("Tx Bytes:\t" << iter->second.txBytes);
    //NS_LOG_UNCOND("Rx Bytes:\t" << iter->second.rxBytes);
    NS_LOG_UNCOND("Throughput " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024 / 1024 << " Mbps");
    }
  if (nTxPkts == 0) {nTxPkts = 1; nRxPkts = 0;} // stop divide by zero errors
  NS_LOG_UNCOND("Total received: " << nRxPkts << "/" << nTxPkts << " (" << (((float) nRxPkts / (float) nTxPkts)*100.0) << "%)");
  std::ofstream out (statsFileName.c_str ());
  out << "rtCorruptionProbability,cbrNodes,nodeSpeed,throughput" << std::endl;
  out << rtCorruptionProbability << "," << nSinks << "," << nodeSpeed << "," << ((float) nRxPkts / (float) nTxPkts) << std::endl;
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
DsdvManetExperiment::InstallInternetStack (std::string tr_name, std::vector<DsdvHelper> &dsdvHelpers)
{

  std::vector<int32_t> ignoreColumns = std::vector<int32_t>();

  for (uint32_t i = 0; i < m_nWifis; i++)
  { 

    DsdvHelper dsdv;
    dsdv.Set ("PeriodicUpdateInterval", TimeValue (Seconds (m_periodicUpdateInterval)));
    dsdv.Set ("SettlingTime", TimeValue (Seconds (m_settlingTime)));

    if (((double)(std::rand() % 100)/100.0)<= m_rtCorruptionProbability)
    { 
      // Ignore column 1 to m_nWifis (default: 50)
      dsdv.Set("IgnoreColumn",IntegerValue((std::rand() % m_nWifis+1)));
    } else {
      dsdv.Set("IgnoreColumn",IntegerValue(0));
    }

    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ((tr_name + ".routes"), std::ios::out);
    dsdv.PrintRoutingTableAllAt (Seconds (m_periodicUpdateInterval), routingStream);

    dsdvHelpers.push_back(dsdv);

  }
  //dsdv.Set ("IgnoreColumns", ObjectVectorValue(ignoreColumns));
  for (uint32_t i = 0; i < m_nWifis; i++)
  {
    InternetStackHelper stack;
    stack.SetRoutingHelper(dsdvHelpers.at(i));
    stack.Install(nodes.Get(i)); 
  }

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  interfaces = address.Assign(devices);
}

void
DsdvManetExperiment::InstallApplications ()
{
  uint32_t clientNode = m_nWifis - 1;
  for (uint32_t i = 0; i < m_nSinks; i++ )
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

