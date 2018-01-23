# cloud-covert-channels
Creating a covert channel in Linux-based cloud container environments.  

## Folders
**proc-locks**  
Covert channel using /proc/locks.  

**proc-locks-no-ack**  
Covert channel using /proc/locks, except the receiver does not ACK the sender for each transmission; Conversely, the sender
does not wait for an ACK from the receiver before it sends the next transmission. This implementation has a much higher data
bandwidth than the above implementation which uses an ACK system. However, it is very possible for the receiver to miss a transmission 
from the sender or re-count the same transmission one or more times.  

**proc-interrupts**  
An attempt to create a covert channel using /proc/interrupts. Channel not completed.  

**proc-meminfo**  
Covert channel using /proc/meminfo. Channel development in progress. Sink should be started slightly
before the source is started. The sink can be run with a "-p" argument to have the recorded data plotted and saved as an image.
The plotting script needs Python 3 and matplotlib.
