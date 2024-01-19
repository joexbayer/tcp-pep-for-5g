# uio-master-joeba

Inital testing topology:
```
  (Sender)                              (Router / PEP)                           (Receiver)
172.16.10.4                     172.16.10.254 - 172.16.11.254                    172.16.11.5
   enp36s0 <---( 100ms delay ) ---> enp36s0           enp24s0 <---( 20ms delay )---> enp36s0
               100Mb htb rate                                   10Mb htb rate
                1,25k bfifo                                      0,025k Bfifo


Hylia:  10.10.10.3
Midna:  10.10.10.6
Majora: 10.10.10.7

Hylia 10.100.36.3 ---- 10.100.36.6 Midna 10.100.67.6 ------ 10.100.67.7 Majora

Ultra Ping:
payload len: 100
send rate kBps: 20


Rate change from:
  1Mb -- 9s --> 10Mb -- 2s --> 50Mb
  
Note: 6Mb file can be transfered from Sender to PEP in 9s.
```

## todo:
Write about 5g in Design chapter 
Finish design chapter and implementation 
test a 100/100 vs 10/120 flow


## Config of router
### install
    Ubuntu Server (22 - Kernel >5.15) 
#### apt
    update
    upgrade
    build-essential
    ifupdown
    net-tools
    ---------
    virtualbox-guest-utils (for shared folder)

### config
    sudo adduser joe vboxsf
    sudo vi /etc/sysctl.conf ? (for router)
        enable ip_forward

### Findings
```
Using the default 20ms config between PEP and Receiver:
Sender -----> PEP -- 20ms --> Receiver.
NO PEP:
Client: 8-11s
Server: 9-12s
PEP:
Client: 3.8s
Server: 5s

Using default 20ms config between client -> router and router -> server:
Sender -- 20ms --> PEP -- 20ms --> Receiver.
NO PEP:
Client: 5.3s
Server: 5.6s
PEP:
Client: 5.0s, 5.1s
Server: 5.4s, 5.5s

Sender -- 8ms (100mbs) --> PEP -- 20ms (10mbs) --> Receiver.
6mb
NO PEP
Client: 5.4s
Server: 5,6s

WITH PEP:
Client: 3.5s
Server: 6.3s

32mb
NO PEP
Client: 29s
Server: 29s

WITH PEP:
Client: 50s
Server: 52s

200mb
NO PEP:
Client: 230
Server: 230

WITH PEP:
Client: 303
Server: 303

#### IF the pep has enough buffe to store entire 32mb
WITH PEP:
Client: 4.8s
Server: 28s


Sender -- 100ms (100mbs) --> PEP -- 20ms (10mbs) --> Receiver.
32mb no pep
CLient: 79s
Server 80s

6mb no pep
Client: 14.5s
Server: 15s
6mb pep
Client: 9s
Server 10s
```
### SSH
    client:
        ssh joe@localhost -p 5555 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
    server:
        ssh joe@localhost -p 5556 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
    router:
        ssh joe@localhost -p 5557 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
        

## TODO
Bandwidth change default
Same host 6 flows vs Different host 6 flows.

10x10 pep and without

### Test 1
100mbit
15mbit -> 70mbit at 18s
file transfer at 5s
20 packets per second.

End times:
BFIFO: 24.6
E2E TCP: 29
PEP TCP: 19.3

### Test 2
6 flows TCP PEP (shared)
LOG [2023-06-16 18:16:13] 32.0mb - 111.592872s
LOG [2023-06-16 18:16:14] 32.0mb - 112.510670s
LOG [2023-06-16 18:16:14] 32.0mb - 112.819391s
LOG [2023-06-16 18:16:14] 32.0mb - 112.887379s
LOG [2023-06-16 18:16:14] 32.0mb - 113.045915s
LOG [2023-06-16 18:16:14] 32.0mb - 113.026711s

6 flow TCP PEP (seperate)
LOG [2023-06-16 18:08:30] 32.0mb - 112.558434s
LOG [2023-06-16 18:08:30] 32.0mb - 112.810743s
LOG [2023-06-16 18:08:30] 32.0mb - 112.812472s
LOG [2023-06-16 18:08:30] 32.0mb - 112.811861s
LOG [2023-06-16 18:08:30] 32.0mb - 113.057965s
LOG [2023-06-16 18:08:30] 32.0mb - 113.081419s

###
1 - 2 flows, 550
3 flows, 700
4 flows, 800
5 - 6 flows, 900
7 flows, 1120
8 flows, 1200
9 flows 1300
4 flows 1400


#### Public key
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDvSXOoDuLKw0ioTIW9tdQpUK0r4KGRgkmZkhwDm7kUGgGtmMjbQo81A74kA7na9ueexbSV/482ZmZcosg3Nwe+isjUINCBCFSXvmhqovv/ynnhn+kBTF03Bxpqciq4LNYViubLj3SnrCle6HU4aoDOx0L3MNyCVInaQTualeLKloA4ZKo/fv7kbzWXAKvClNH6v6axtQl58wfIt1ABEROcl0myY22D0Mn6yQCm28qqb5mqstVGWjTVxJPhfjtVp9NVXyz9ZPfnca/Tdbj/XRdZDtE3fsxaKAUdPs+VRwqNSAzkXH40Wz1t1dUMK/ahxZ5SCZkyBtqq4qE8e/A0SwUfuixH48Z2uVOjPNkNy0s+tu9NJjenLzLIFczRU4K8Il8U3roUsE7/m0mHqld74M/Ti3/U4SmKgcK9p28XR7LOJmY3sdRwt4THxOuQa3J6xJZkPAiE+3Oq3qME6mp2s08/pB55NY+6v3pXY/xVOSCkbkQU1CHA4YylZfT9kYbwwd8= joe@regolith
