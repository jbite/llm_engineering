#### MPLS VPN

##### LAB

VRF下，各種動態路由協議配置

* RIP

```
router rip 
address family ipv4 vrf VRFNAME
version 2
network 0.0.0.0
```

* OSPF

```cisco
router ospf 1 vrf VRFNAME
router-id 1.1.1.1
network 0.0.0.0 0.0.0.0 a 0
```



* EIGRP
* BGP