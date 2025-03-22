
# TCP Performance Enhancing Proxy Support For Non-interactive Applications

## Performance Enhancing Proxy for 5G mmWave Networks

This repository contains the practical implementation of my master's thesis:  
**"TCP Performance Enhancing Proxy Support For Non-interactive Applications"**

In fluctuating bandwidth environments — particularly within 5G mmWave networks — maintaining optimal performance is challenging. This thesis presents a novel Performance Enhancing Proxy (PEP) designed specifically to optimize non-interactive traffic, such as large file transfers or background sync, while protecting interactive traffic like video calls and real-time messaging from congestion-induced latency.

### Key Highlights:

- **Bandwidth Optimization:** Uses large socket buffers and active queue management to maximize throughput.
- **Targeted for 5G mmWave:** Evaluated using real-world 5G trace data under dynamic bandwidth conditions.
- **Traffic-Aware Design:** Differentiates between interactive and non-interactive traffic to balance performance and responsiveness.
- **Ethical Considerations:** Addresses transparency, user consent, and privacy in the context of network-layer proxies.

<p align="center">
    <img src="https://github.com/joexbayer/tcp-pep-for-5g/blob/main/graphs/throughput.png?raw=true">
</p>

| | |
|:-------------------------:|:-------------------------:|
| <img src="https://github.com/joexbayer/tcp-pep-for-5g/blob/main/graphs/mobb_throughput.png?raw=true"> | <img src="https://github.com/joexbayer/tcp-pep-for-5g/blob/main/graphs/mobb_impact.png?raw=true">

| | |
|:-------------------------:|:-------------------------:|
| <img src="https://github.com/joexbayer/tcp-pep-for-5g/blob/main/graphs/static_link.png?raw=true"> | <img src="https://github.com/joexbayer/tcp-pep-for-5g/blob/main/graphs/static_link_impact.png?raw=true">