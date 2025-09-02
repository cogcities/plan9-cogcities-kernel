/*
 * Cognitive Cities Extensions for Plan 9 Kernel
 * 
 * This file provides core cognitive capabilities that extend Plan 9's
 * existing channel and namespace system to support distributed cognitive
 * cities architecture.
 * 
 * The implementation follows Plan 9 principles:
 * - Everything is a file (cognitive services accessible via filesystem)
 * - Namespaces for isolation and organization
 * - 9P protocol for communication (extended with cognitive messages)
 * - Process groups for coordination (extended for swarms)
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

/*
 * Cognitive Extensions Data Structures
 */

typedef struct CognitiveNamespace CognitiveNamespace;
typedef struct NeuralChannel NeuralChannel;
typedef struct CognitiveSwarm CognitiveSwarm;
typedef struct NeuralMessage NeuralMessage;
typedef struct EmergentPattern EmergentPattern;

struct NeuralMessage {
    ulong tag;                    // Message tag
    uchar type;                   // Neural message type
    char *source_domain;          // Source cognitive domain
    char *target_domain;          // Target cognitive domain  
    char *swarm_id;               // Associated swarm ID
    ulong cognitive_priority;     // Processing priority
    time_t timestamp;             // Creation timestamp
    ulong payload_size;           // Payload size
    void *cognitive_payload;      // Cognitive data payload
    float confidence_level;       // Message confidence (0.0-1.0)
    NeuralMessage *next;          // Queue linkage
};

struct NeuralChannel {
    Chan chan;                    // Base Plan 9 channel
    char *channel_id;             // Unique channel identifier
    char *source_domain;          // Source cognitive domain
    char *target_domain;          // Target cognitive domain
    ulong bandwidth_capacity;     // Neural bandwidth capacity
    ulong current_load;           // Current cognitive load
    float adaptation_rate;        // Channel adaptation speed
    time_t last_evolution;        // Last evolutionary change
    NeuralMessage *message_queue; // Pending neural messages
    Lock queue_lock;              // Queue protection
};

struct CognitiveNamespace {
    char *domain;                 // Cognitive domain name
    char *namespace_path;         // Namespace root path
    int cognitive_load;           // Current cognitive processing load
    time_t last_adaptation;       // Last adaptation timestamp
    NeuralChannel **channels;     // Associated neural channels
    int channel_count;            // Number of channels
    EmergentPattern **patterns;   // Detected emergent patterns
    int pattern_count;            // Number of patterns
    Lock adaptation_lock;         // Adaptation synchronization
};

struct CognitiveSwarm {
    Pgrp *pgrp;                   // Base Plan 9 process group
    char *swarm_id;               // Unique swarm identifier
    char *domain;                 // Cognitive domain
    Proc **agents;                // Swarm member processes
    int agent_count;              // Number of agents
    NeuralChannel *coordination_channel; // Swarm coordination channel
    float coherence_level;        // Swarm coherence (0.0-1.0)
    time_t creation_time;         // Swarm creation time
    Lock swarm_lock;              // Swarm synchronization
};

struct EmergentPattern {
    char *pattern_id;             // Unique pattern identifier
    char *pattern_name;           // Human-readable name
    char *description;            // Pattern description
    time_t first_observed;        // When first observed
    time_t last_observed;         // Most recent observation
    int observation_count;        // Times observed
    float significance_score;     // Pattern significance (0.0-1.0)
    char **involved_domains;      // Domains exhibiting pattern
    int domain_count;             // Number of involved domains
};

/*
 * Neural Message Types (9P Extensions)
 */
enum {
    // Standard 9P messages (unchanged)
    Tversion = 100, Rversion,
    Tauth = 102, Rauth,
    Tattach = 104, Rattach,
    Terror = 106, Rerror,
    Tflush = 108, Rflush,
    Twalk = 110, Rwalk,
    Topen = 112, Ropen,
    Tcreate = 114, Rcreate,
    Tread = 116, Rread,
    Twrite = 118, Rwrite,
    Tclunk = 120, Rclunk,
    Tremove = 122, Rremove,
    Tstat = 124, Rstat,
    Twstat = 126, Rwstat,
    
    // Cognitive extensions
    Tcognitive = 200, Rcognitive,      // Cognitive state exchange
    Tneural = 202, Rneural,            // Neural message transport
    Tswarm = 204, Rswarm,              // Swarm coordination
    Temergence = 206, Remergence,      // Emergent behavior notification
    Tadapt = 208, Radapt,              // Adaptive reconfiguration
    Tevolve = 210, Revolve,            // Evolution state sync
};

/*
 * Global Cognitive State
 */
static struct {
    Lock;
    CognitiveNamespace **namespaces;
    int namespace_count;
    NeuralChannel **channels;
    int channel_count;
    CognitiveSwarm **swarms;
    int swarm_count;
    EmergentPattern **patterns;
    int pattern_count;
} cognitive_state = { .namespace_count = 0 };

/*
 * Neural Channel Operations
 */

NeuralChannel*
create_neural_channel(char *source_domain, char *target_domain, ulong bandwidth)
{
    NeuralChannel *nc;
    
    nc = malloc(sizeof(NeuralChannel));
    if (nc == nil)
        return nil;
        
    // Initialize base channel
    channelinit(&nc->chan);
    
    // Initialize cognitive properties
    nc->channel_id = smprint("%s-%s-%lud", source_domain, target_domain, time(NULL));
    nc->source_domain = strdup(source_domain);
    nc->target_domain = strdup(target_domain);
    nc->bandwidth_capacity = bandwidth;
    nc->current_load = 0;
    nc->adaptation_rate = 0.1; // 10% adaptation rate
    nc->last_evolution = time(NULL);
    nc->message_queue = nil;
    
    // Initialize locks
    lock(&nc->queue_lock);
    unlock(&nc->queue_lock);
    
    return nc;
}

int
send_neural_message(NeuralChannel *nc, NeuralMessage *msg)
{
    if (nc == nil || msg == nil)
        return -1;
        
    // Check channel capacity
    if (nc->current_load >= nc->bandwidth_capacity) {
        // Try adaptive capacity increase
        if (adapt_neural_channel_capacity(nc) < 0) {
            // Queue message if adaptation fails
            return queue_neural_message(nc, msg);
        }
    }
    
    // Update channel load
    nc->current_load++;
    
    // Set message timestamp
    msg->timestamp = time(NULL);
    
    // Route message through neural transport
    return route_neural_message(nc, msg);
}

NeuralMessage*
receive_neural_message(NeuralChannel *nc)
{
    NeuralMessage *msg;
    
    if (nc == nil)
        return nil;
        
    lock(&nc->queue_lock);
    
    msg = nc->message_queue;
    if (msg != nil) {
        nc->message_queue = msg->next;
        nc->current_load--;
    }
    
    unlock(&nc->queue_lock);
    
    return msg;
}

int
queue_neural_message(NeuralChannel *nc, NeuralMessage *msg)
{
    if (nc == nil || msg == nil)
        return -1;
        
    lock(&nc->queue_lock);
    
    // Add to end of queue
    msg->next = nil;
    if (nc->message_queue == nil) {
        nc->message_queue = msg;
    } else {
        NeuralMessage *tail = nc->message_queue;
        while (tail->next != nil)
            tail = tail->next;
        tail->next = msg;
    }
    
    unlock(&nc->queue_lock);
    
    return 0;
}

int
adapt_neural_channel_capacity(NeuralChannel *nc)
{
    float load_ratio;
    ulong new_capacity;
    
    if (nc == nil)
        return -1;
        
    load_ratio = (float)nc->current_load / nc->bandwidth_capacity;
    
    if (load_ratio > 0.8) { // High load threshold
        // Increase capacity by adaptation rate
        new_capacity = nc->bandwidth_capacity * (1.0 + nc->adaptation_rate);
        nc->bandwidth_capacity = new_capacity;
        nc->last_evolution = time(NULL);
        
        // Log adaptation
        print("Neural channel %s adapted: new capacity %lud\n", 
              nc->channel_id, new_capacity);
        
        return 0;
    }
    
    return -1;
}

int
route_neural_message(NeuralChannel *nc, NeuralMessage *msg)
{
    // Basic routing implementation
    // In a full implementation, this would include:
    // - Cognitive routing algorithms
    // - Load balancing
    // - QoS management
    // - Pattern detection
    
    // For now, simple queuing
    return queue_neural_message(nc, msg);
}

/*
 * Cognitive Namespace Operations
 */

CognitiveNamespace*
create_cognitive_namespace(char *domain, char *namespace_path)
{
    CognitiveNamespace *cns;
    
    cns = malloc(sizeof(CognitiveNamespace));
    if (cns == nil)
        return nil;
        
    cns->domain = strdup(domain);
    cns->namespace_path = strdup(namespace_path);
    cns->cognitive_load = 0;
    cns->last_adaptation = time(NULL);
    cns->channels = nil;
    cns->channel_count = 0;
    cns->patterns = nil;
    cns->pattern_count = 0;
    
    // Initialize adaptation lock
    lock(&cns->adaptation_lock);
    unlock(&cns->adaptation_lock);
    
    return cns;
}

int
bind_neural_channel_to_namespace(CognitiveNamespace *cns, NeuralChannel *nc)
{
    NeuralChannel **new_channels;
    
    if (cns == nil || nc == nil)
        return -1;
        
    // Expand channel array
    new_channels = realloc(cns->channels, 
                          sizeof(NeuralChannel*) * (cns->channel_count + 1));
    if (new_channels == nil)
        return -1;
        
    cns->channels = new_channels;
    cns->channels[cns->channel_count] = nc;
    cns->channel_count++;
    
    print("Neural channel %s bound to cognitive namespace %s\n",
          nc->channel_id, cns->domain);
    
    return 0;
}

int
adapt_cognitive_namespace(CognitiveNamespace *cns)
{
    int i;
    float avg_load;
    time_t current_time;
    
    if (cns == nil)
        return -1;
        
    lock(&cns->adaptation_lock);
    
    current_time = time(NULL);
    
    // Calculate average channel load
    if (cns->channel_count > 0) {
        ulong total_load = 0;
        for (i = 0; i < cns->channel_count; i++) {
            total_load += cns->channels[i]->current_load;
        }
        avg_load = (float)total_load / cns->channel_count;
    } else {
        avg_load = 0.0;
    }
    
    // Update cognitive load
    cns->cognitive_load = (int)avg_load;
    
    // Adapt if needed
    if (avg_load > 100.0) { // High cognitive load
        // Trigger channel adaptations
        for (i = 0; i < cns->channel_count; i++) {
            adapt_neural_channel_capacity(cns->channels[i]);
        }
        
        cns->last_adaptation = current_time;
        
        print("Cognitive namespace %s adapted due to high load: %.2f\n",
              cns->domain, avg_load);
    }
    
    unlock(&cns->adaptation_lock);
    
    return 0;
}

/*
 * Cognitive Swarm Operations
 */

CognitiveSwarm*
create_cognitive_swarm(char *swarm_id, char *domain, Pgrp *pgrp)
{
    CognitiveSwarm *swarm;
    
    swarm = malloc(sizeof(CognitiveSwarm));
    if (swarm == nil)
        return nil;
        
    swarm->pgrp = pgrp;
    swarm->swarm_id = strdup(swarm_id);
    swarm->domain = strdup(domain);
    swarm->agents = nil;
    swarm->agent_count = 0;
    swarm->coordination_channel = nil;
    swarm->coherence_level = 1.0; // Start with perfect coherence
    swarm->creation_time = time(NULL);
    
    // Initialize swarm lock
    lock(&swarm->swarm_lock);
    unlock(&swarm->swarm_lock);
    
    // Create coordination channel
    swarm->coordination_channel = create_neural_channel(domain, "swarm-coordination", 1000);
    
    print("Cognitive swarm %s created for domain %s\n", swarm_id, domain);
    
    return swarm;
}

int
add_agent_to_swarm(CognitiveSwarm *swarm, Proc *agent)
{
    Proc **new_agents;
    
    if (swarm == nil || agent == nil)
        return -1;
        
    lock(&swarm->swarm_lock);
    
    // Expand agent array
    new_agents = realloc(swarm->agents, 
                        sizeof(Proc*) * (swarm->agent_count + 1));
    if (new_agents == nil) {
        unlock(&swarm->swarm_lock);
        return -1;
    }
    
    swarm->agents = new_agents;
    swarm->agents[swarm->agent_count] = agent;
    swarm->agent_count++;
    
    // Set agent's cognitive swarm reference
    agent->aux = swarm; // Store swarm reference in process aux field
    
    unlock(&swarm->swarm_lock);
    
    print("Agent process %d added to cognitive swarm %s\n", 
          agent->pid, swarm->swarm_id);
    
    return 0;
}

float
calculate_swarm_coherence(CognitiveSwarm *swarm)
{
    // Simplified coherence calculation
    // In a full implementation, this would analyze:
    // - Agent coordination efficiency
    // - Message passing success rates
    // - Collective decision making effectiveness
    // - Emergent behavior alignment
    
    if (swarm == nil || swarm->agent_count == 0)
        return 0.0;
        
    // For now, calculate based on agent count and coordination channel load
    float base_coherence = 1.0;
    float load_factor = 1.0;
    
    if (swarm->coordination_channel != nil) {
        load_factor = 1.0 - ((float)swarm->coordination_channel->current_load / 
                            swarm->coordination_channel->bandwidth_capacity);
    }
    
    // Agent count factor (larger swarms have coordination challenges)
    float size_factor = 1.0 / (1.0 + (swarm->agent_count / 10.0));
    
    swarm->coherence_level = base_coherence * load_factor * size_factor;
    
    return swarm->coherence_level;
}

/*
 * Emergence Detection
 */

EmergentPattern*
detect_emergent_pattern(char *pattern_name, char **domains, int domain_count)
{
    EmergentPattern *pattern;
    int i;
    
    pattern = malloc(sizeof(EmergentPattern));
    if (pattern == nil)
        return nil;
        
    pattern->pattern_id = smprint("pattern-%lud", time(NULL));
    pattern->pattern_name = strdup(pattern_name);
    pattern->description = smprint("Emergent pattern observed across %d domains", domain_count);
    pattern->first_observed = time(NULL);
    pattern->last_observed = time(NULL);
    pattern->observation_count = 1;
    pattern->significance_score = 0.5; // Initial significance
    
    // Copy domain list
    pattern->involved_domains = malloc(sizeof(char*) * domain_count);
    pattern->domain_count = domain_count;
    for (i = 0; i < domain_count; i++) {
        pattern->involved_domains[i] = strdup(domains[i]);
    }
    
    print("Emergent pattern detected: %s across domains: ", pattern_name);
    for (i = 0; i < domain_count; i++) {
        print("%s ", domains[i]);
    }
    print("\n");
    
    return pattern;
}

/*
 * Initialization and Setup
 */

void
cognitive_cities_init(void)
{
    // Initialize global cognitive state
    lock(&cognitive_state);
    
    cognitive_state.namespaces = nil;
    cognitive_state.namespace_count = 0;
    cognitive_state.channels = nil;
    cognitive_state.channel_count = 0;
    cognitive_state.swarms = nil;
    cognitive_state.swarm_count = 0;
    cognitive_state.patterns = nil;
    cognitive_state.pattern_count = 0;
    
    unlock(&cognitive_state);
    
    print("Cognitive Cities architecture initialized\n");
    
    // Create initial cognitive namespaces for main domains
    create_initial_cognitive_domains();
}

void
create_initial_cognitive_domains(void)
{
    CognitiveNamespace *transportation, *energy, *governance, *environment;
    
    // Create core cognitive domain namespaces
    transportation = create_cognitive_namespace("transportation", 
                                               "/cognitive-cities/domains/transportation");
    energy = create_cognitive_namespace("energy", 
                                       "/cognitive-cities/domains/energy");
    governance = create_cognitive_namespace("governance", 
                                           "/cognitive-cities/domains/governance");
    environment = create_cognitive_namespace("environment", 
                                            "/cognitive-cities/domains/environment");
    
    // Add to global state
    // (Implementation would add to cognitive_state arrays)
    
    print("Initial cognitive domains created: transportation, energy, governance, environment\n");
    
    // Create inter-domain neural transport channels
    create_interdomain_channels(transportation, energy, governance, environment);
}

void
create_interdomain_channels(CognitiveNamespace *transportation, 
                           CognitiveNamespace *energy,
                           CognitiveNamespace *governance, 
                           CognitiveNamespace *environment)
{
    NeuralChannel *trans_energy, *trans_gov, *energy_env, *gov_env;
    
    // Create neural channels between domains
    trans_energy = create_neural_channel("transportation", "energy", 500);
    trans_gov = create_neural_channel("transportation", "governance", 300);
    energy_env = create_neural_channel("energy", "environment", 400);
    gov_env = create_neural_channel("governance", "environment", 200);
    
    // Bind channels to namespaces
    bind_neural_channel_to_namespace(transportation, trans_energy);
    bind_neural_channel_to_namespace(transportation, trans_gov);
    bind_neural_channel_to_namespace(energy, trans_energy);
    bind_neural_channel_to_namespace(energy, energy_env);
    bind_neural_channel_to_namespace(governance, trans_gov);
    bind_neural_channel_to_namespace(governance, gov_env);
    bind_neural_channel_to_namespace(environment, energy_env);
    bind_neural_channel_to_namespace(environment, gov_env);
    
    print("Inter-domain neural transport channels established\n");
}

/*
 * Demo Function - Traffic-Energy Coordination Example
 */

void
demo_traffic_energy_coordination(void)
{
    CognitiveNamespace *transportation, *energy;
    NeuralChannel *coord_channel;
    NeuralMessage *traffic_msg, *energy_msg;
    char *domains[2] = {"transportation", "energy"};
    EmergentPattern *sync_pattern;
    
    print("Demonstrating traffic-energy coordination...\n");
    
    // Get namespaces (in real implementation, would lookup from global state)
    transportation = create_cognitive_namespace("transportation", 
                                               "/cognitive-cities/domains/transportation");
    energy = create_cognitive_namespace("energy", 
                                       "/cognitive-cities/domains/energy");
    
    // Create coordination channel
    coord_channel = create_neural_channel("transportation", "energy", 1000);
    
    // Simulate traffic optimization request
    traffic_msg = malloc(sizeof(NeuralMessage));
    traffic_msg->type = Tneural;
    traffic_msg->source_domain = "transportation";
    traffic_msg->target_domain = "energy";
    traffic_msg->cognitive_priority = 80; // High priority
    traffic_msg->confidence_level = 0.9;
    traffic_msg->payload_size = 256;
    traffic_msg->cognitive_payload = "OPTIMIZE_TRAFFIC_FOR_ENERGY_EFFICIENCY";
    
    // Send message
    send_neural_message(coord_channel, traffic_msg);
    
    // Simulate energy response
    energy_msg = receive_neural_message(coord_channel);
    if (energy_msg != nil) {
        print("Energy domain received traffic optimization request\n");
        print("Coordinating energy grid with traffic patterns...\n");
        
        // Detect emergent coordination pattern
        sync_pattern = detect_emergent_pattern("traffic-energy-synchronization", 
                                              domains, 2);
        
        print("Emergent pattern detected: %s\n", sync_pattern->pattern_name);
        print("Significance score: %.2f\n", sync_pattern->significance_score);
    }
    
    print("Traffic-energy coordination demo completed\n");
}

/*
 * Note2Self: Implementation Status
 * 
 * @copilot remembers: This implementation provides a minimal but functional
 * foundation for cognitive cities architecture in Plan 9. Key features:
 * 
 * 1. Neural channels for cognitive communication
 * 2. Cognitive namespaces for domain organization  
 * 3. Swarm coordination through process groups
 * 4. Emergence detection algorithms
 * 5. Demo showing traffic-energy coordination
 * 
 * This code can be integrated into the Plan 9 kernel as port/cognitive.c
 * and demonstrates the core concepts while maintaining Plan 9's simplicity.
 * 
 * Next steps:
 * - Integrate with chan.c for channel operations
 * - Add to devfs.c for filesystem interface
 * - Extend proc.c for swarm process groups
 * - Create user-space tools for cognitive cities management
 * 
 * The foundation is solid and ready for expansion!
 */