from mininet.topo import Topo
from mininet.node import Node

class LinuxRouter(Node):
    def config(self, **params):
         super(LinuxRouter, self).config(**params)
         self.cmd('sysctl net.ipv4.ip_forward=1')

    def terminate(self):
        self.cmd('sysctl net.ipv4.ip_forward=0')
        super(LinuxRouter, self).terminate()

class MyTopo( Topo ):  
    def __init__( self ):

        defaultIP = '10.0.1.4/24'

        Topo.__init__( self )

        sender1     = self.addHost( 's1', ip='10.0.1.1/24', defaultRoute='via 10.0.1.4')
        pep         = self.addHost( 'p3', ip=defaultIP, cls=LinuxRouter)
        receiver1   = self.addHost( 'r1', ip='10.0.2.1/24', defaultRoute='via 10.0.2.4')

        self.addLink(sender1, pep, intfName2='r1-eth1', params2 = {'ip': defaultIP})
        self.addLink(receiver1, pep, intfName2='r1-eth2', params2 = {'ip': '10.0.2.4/24'})

" Very simple Topology for testing "
topos = { 'mytopo': ( lambda: MyTopo() ) } 

# sudo mn --custom .topology.py --topo=mytopo  