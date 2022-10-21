# $DS-A^2RID$: Anonymous Direct Authentication and Remote Identification of Commercial Drones
This repository contains the code of the implementation of an Anonymous Direct Authentication and Remote Identification protocol for Commercial Drones, based on the anonymous group signature scheme introduced by Derler et al.
The scheme uses a
* NIZK scheme --> Schnorr's nizk protocol[^2]
* DS scheme --> BLS signatures [^3]
* PKE scheme --> ECIES [^4]

## Directory structure
* `drone`: An Arduino demo application illustrating the CPA and CCA2 version of the RemoteID scheme in different scenarios :
  - `cca2`: is the cryptographic Adaptive chosen-ciphertext attack secure version called DS-CCA2-A^2RID
    -  setup, verify, open are performed on desktop while sign phase is performed on a drone.
      - `original`: without pre comp.
      - `pre-comp`: with pre comp.
  - `cpa`: is the Chosen-plaintext attack secure version called DS-CPA-A^2RID
    -  setup, verify, open are performed on the desktop while sign phase is performed on a drone.
      - `original`: without pre comp.
      - `pre-comp`: with pre comp.
* `include`: The headers required to run the demo and desktop version.
* `proverif`: A proverif proof proving that this satisfies the requirements set by the FAA.
* `src`: Code of the schemes used (nizk, ds, pke)
* `desktop`: Desktop version of the application mostly used to show correctness (contains DS-CCA2-A^2RID aswell as DS-CPA-A^2RID)

## How to Use
To clone and run the desktop version type:
```bash
# go to your directory of choice 
git clone https://github.com/EvaWisse/RemoteID_gs.git
```
to run the cpa version type
```bash
# compile
make cca2_desktop

# run
./cca2_desktop
```

to run the cca2 version type
```bash
# compile
make cpa_desktop

# run
./cpa_desktop
```
To use the drone application, download the repo as explained before. Select in ```drone/setting.h``` the use pre-computations by setting ```#define PRE_COMP```.
If commented out no pre-computations are performed.
Select in ```drone/setting.h``` which version of signature you whish to use,  ```#define CCA2_SIGNATURE``` indicates the use of CCA2 anonymous signatures. 
If commented out the use of CPA anonymous signatures is assumed.  To run the program one must type: 
```bash
# Create group and join 
make setup_join
./setup_join.out
```
Now one must upload ```cca2_sign.cpp``` or ```cpa_sign.cpp``` to the device in combination with ```drone_const.h``` and the Miracl library if no pre-comp. is used. In case of pre-comp.one must upload ```cca2_sign_pre.cpp``` or ```cpa_sign_pre.cpp``` to the device in combination with ```drone_const.h``` and the Miracl library
After a broadcast is received this needs to stores in ```broadcast.txt```. The broadcast can be verified and opened by:
```bash
# Check broadcast
make verify_open
./verify_open.out 
```
[^1]: Fuchsbauer, G., Hanser, C., & Slamanig, D. (2019). Structure-Preserving Signatures on Equivalence Classes and Constant-Size Anonymous Credentials. Journal of Cryptology, 32(2), 498–546. https://doi.org/10.1007/s00145-018-9281-4
[^2]: Hao, F. (2017). Schnorr Non-interactive Zero-Knowledge Proof (Issue 8235, pp. 1–13). RFC Editor. https://doi.org/10.17487/RFC8235
[^3]: Boneh, D., Gorbunov, S., Wahby, R. S., Wee, H., Wood, C. A., & Zhang, Z. (2022). BLS Signatures (Issue draft-irtf-cfrg-bls-signature-05). Internet Engineering Task Force. https://datatracker.ietf.org/doc/draft-irtf-cfrg-bls-signature/05/
[^4]: Gayoso Martínez, V., Encinas, L. H., & Sánchez Ávila, C. (2010). A Survey of the Elliptic Curve Integrated Encryption Scheme. Journal of Computer Science and Engineering, 2(2), 7. http://sites.google.com/site/jcseuk/
