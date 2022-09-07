# remote_id_gs 
This repository contains a implementation of REMOTE ID based on the group signature scheme introduced by Derler et al. [^1]
Based on the a:
* NIZK scheme -->
* DS scheme -->
* PKE scheme -->

## Directory structure
* 'demo': A demo application illustrating the CPA and CCA2 version of the REMOTE ID scheme in different scenarios:
  - 'cca2': cryptographic Adaptive chosen-ciphertext attack secure version
    - 'desktop': cca2 version that solely runs on your desktop.
    - 'hybrid': setup, verify, open are performed on dekstop while sign phase is performed on drone.
  - 'cpa': Chosen-plaintext attack secure version
    - 'desktop': cpa version that solely runs on your desktop.
    - 'hybrid': setup, verify, open are performed on dekstop while sign phase is performed on drone.
      - 'orginal': 
      - 'pre-comp': 
* 'include': The headers required to run the demo.
* 'proverif': A proverif proof proving that this satisfies the requirements set by the FAA.
* 'src': Code of the schemes used (nizk, ds, ple)
* 'test': Seperate testing code testing the indivalue code sections

[^1]: add link