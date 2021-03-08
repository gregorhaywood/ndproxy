# README


MLD must be disabled. NSs are sent to solicited-node multicast addresses. With 
MLD, these packets would not be sent to our interface. The interface must also 
be in promiscuous mode so that is does not discard NSs destined for other 
nodes. These can both be viewed as optimisations that rely on an assumption we 
are breaking.
