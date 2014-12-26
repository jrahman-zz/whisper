namespace whisper rpc

enum WhisperStatus {
    DOWN_TO_UP = 8,
    UP_TO_DOWN = 4,
    UP = 2,
    DOWN = 1
}

enum AddressFamily {
    IPV6 = 2,
    IPV4 = 1,
}

// Stores information on network interfaces for a given node
// We use this in the event that a node has multiple network interfaces
// either for performance or resilience reasons
// To be assured of a dead node, we try contacting on any interface possible
// and only if all interfaces are bad, do we declare that node as suspect
struct NodeInterface {
    
    // String address, IPv4 or IPv6, not hostname
    1: string address,

    // Numeric port number
    2: i32 port
}

struct Node {
    
    // (Hopefully) unique, random identifier
    1: string guid,

    // Communications interface the node can be reached via
    2: NodeInterface interface,

    // The current known status, as far as this node is concerned
    4: Status currentStatus,

    // The current logical timestamp for the given node
    8: i64 timestamp
}

struct GossipState {
    
    // Sender of this GossipState
    1: Node sender,

    // Set of known live GossipNodes
    4: set<Node> liveNodes,

    // Set of potentially dead GossipNodes based on purely local information
    8: set<Node> suspectNodes,

    // Set of agreed upon dead GossipNodes based on multi-node consensus
    16: set<Node> deadNodes,
}

// Generic service representing a server that participates in the Gossip protcol
service Service {

    // Generic health ping
    void ping(),

    // Join Gossip protocol by sending information about myself
    set<State> joinCluster(1: Node self),

    // Send health update to other Gossip Node
    State sendUpdate(1: State state),
}
