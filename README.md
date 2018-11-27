# cloud-covert-channels
Creating covert channels in Linux-based cloud container environments. The paper accompanying this research is titled "A Study on the Security Implications of Information Leakages in Container Clouds." It is  published in *IEEE Transactions on Dependable and Secure Computing*, 2018. Clean up of the code is also forthcoming.  

## Channels
**proc-locks**  
Covert channel using /proc/locks. It implements an ACK system that reduces 
channel bandwidth, but improves channel reliability.

**proc-locks-no-ack**  
Covert channel using /proc/locks, except the receiver does not ACK the sender 
for each transmission; Conversely, the sender does not wait for an ACK from the 
receiver before it sends the next transmission. This implementation has a much 
higher data bandwidth than the above implementation. However, it is very 
possible for the receiver to miss a transmission from the sender or re-count 
the same transmission one or more times.  

**proc-meminfo**  
Covert channel using /proc/meminfo. The sink should be started slightly before 
the source is started. The sink can be run with a "-p" argument to have the 
recorded data plotted and saved as an image. The plotting script needs Python 3 
and matplotlib.
