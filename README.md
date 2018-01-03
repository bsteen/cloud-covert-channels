#cloud-covert-channels
Creating a covert channel in Linux-based cloud environments

##Folders
**proc-locks**  
Covert channel using /proc/locks.

**proc-locks-mo-ack**  
Covert channel using /proc/locks, except the receiver does not ACK the sender for each transmission; Conversely, the sender
does not wait for an ACK from the receiver before it sends the next transmission. This implementation has a much higher data
bandwidth than the above implementation which uses an ACK system. However, it is very possible for the receiver to miss a transmission 
from the sender or re-count the same transmission one or more times.