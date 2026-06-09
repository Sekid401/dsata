dSATA - Get Prepared to get the SATA experience Digitally

Features - NCQ, Query Just like real SATA (note: it's literally just NCQ from temu)

HighLights:

Suspend/resume
NCQ
Read/write Caps
Portability
SMART
dssign

dSATA Portable Executable - DPE, Share dSATA to People who don't know how to compile and shit

Where it would work best:

SATA
mSATA
Micro SATA
eSATA
any SATA stuff

Where it could work:

NVMe
eMMC (do NOT use dSATA Express on this, seriously it goes from 30mb to 10kb)
HDD

Get Started:

Grab the Latest zip

Run make in your Terminal

Wait till your binary is Compiled

Run ./tests, ./dsataserver or ./dpe for dSATA Features, To test run ./tests, to do dSATA features use ./dpe or dpe

----- Test Results -----

=== test_local_write_read ===
PASS: write/read match
=== test_speed_limits ===
Write speed: 25.47 MB/s (limit: 557 MB/s)
Read speed:  394.26 MB/s (limit: 803 MB/s)
=== test_smart ===
=== dSATA SMART Report ===
Reads:          1
Writes:         1
Bytes Read:     15
Bytes Written:  15
Cache Hits:     1
Cache Misses:   0
Cache Evictions:0
NCQ Processed:  0
NCQ Reordered:  0
Trimmed Blocks: 0
Read Errors:    0
Write Errors:   0
Uptime (s):     0
Health:         100/100
PASS: SMART report generated
=== test_ncq ===
PASS: NCQ write/read correct (processed: 2, reordered: 0)
=== test_trim ===
PASS: TRIM zeroed block (trimmed blocks: 1)
=== test_hotswap ===
PASS: hot swapped to 'hotswap_b'
=== test_suspend_resume ===
Suspended: yes
Resumed:   no
PASS: suspend blocked I/O correctly
PASS: write after resume ok
=== test_port_multiplier ===
PASS: vol1 registered
PASS: vol2 registered
=== test_web_connect ===
DSataWeb created, connected: no
PASS: DSataWeb init ok (skipping actual connect, no server)
=== test_dssign ===
Type:      dSATA
Signature: Digital SATA Drive
Caps:      NCQ,TRIM,SMART,HOTSWAP,PM
Has NCQ:   yes
PASS: dssign ok
=== test_power_states ===
Initial state: ACTIVE
After suspend: SLUMBER
After resume:  ACTIVE
PASS: power states ok
=== test_soft_reset ===
PASS: soft reset completed
PASS: write after reset ok
=== test_comreset ===
PASS: COMRESET completed
PASS: write after COMRESET ok
=== test_staggered_spinup ===
PASS: staggered spinup completed for 3 drives
=== test_error_recovery ===
Error log entries: 0
Recovery failed:   no
PASS: error recovery state ok
=== test_ssc ===
SSC enabled: no
PASS: SSC flag accessible (default: off)
=== test_devslp ===
Initial state: ACTIVE
PASS: DEVSLP state exists in power manager
=== test_sector_size_512 ===
Sector size: 512 bytes
PASS: 512 byte sector IDENTIFY ok
=== test_sector_size_4096 ===
Sector size: 4096 bytes
PASS: Advanced Format 4K sector
=== test_sata_express ===
SATA Express: yes
Write speed: 0.38 MB/s (limit: 1500 MB/s)
PASS: SATA Express speed profile active
=== test_fis_routing ===
PASS: FIS routed to vol1
PASS: FIS routed to vol2
PASS: vol1 data correct
PASS: vol2 data correct
=== test_ncq_overlap ===
PASS: NCQ overlap all data correct (processed: 3)
=== test_dahci_identify ===
Model: Digital SATA Drive
LBA48 sectors: 0
PASS: IDENTIFY ok
=== test_dahci_read_write ===
PASS: dAHCI read/write match
=== test_dahci_irq ===
IRQ: port=0 is=0x00000001
PASS: IRQ fired after command
=== test_dahci_port_regs ===
SIG:  0x00000101 (expected 0x00000101)
SSTS: 0x00000003 (expected 0x00000003)
TFD:  0x00000040
CAP:  0x40201F05
PASS: port registers correct
=== test_dahci_port_reset ===
PASS: soft port reset ok
PASS: COMRESET port reset ok
=== test_dahci_dma ===
DMA write issued, tag=0
DMA drained, IRQ fired: yes, last IS: 0x00000008
PASS: DMA write verified on readback
=== test_dahci_ncq_abort ===
Abort tag 0: PASS: aborted
PASS: NCQ abort test done
=== test_dahci_ncq_depth ===
FORTIFY: pthread_mutex_lock called on a destroyed mutex (0x7fead87e90)
NCQ queue depth reported: 32 (expected 32)
PASS: NCQ depth correct

All tests done.

----- Test Results End -----

Why not run a OS without dAHCI?

- dAHCI -

A feature that controls the dSATA drive and allows OSes to boot in a VM, this feature has support for running things like DOS

- dAHCI -

- QnA -

How does it work? Answer: it simply uses YOUR drive and set $DSATAHOME to create dSATA volumes and such, It also enforces caps and has SATA semantics

What would happen if I run dSATA in a NVMe? Answer: it will throttle so hard that you get 100mb write speed

Is this a full SATA implementation? Answer: yes and no that's my answer 

- QnA -

- Coolest Features -

- DPE - a Feature that turns your dSATA portable instantly -

- DAHCI - Allows you to do more things with dSATA! -

- Coolest Features end -

What are you waiting for? Microsoft releasing windows 12 and windows 12 actually being better than windows 11? Get dSATA Today!
