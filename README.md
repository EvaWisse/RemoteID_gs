# A2RID
This repository contains a implementation of RemoteID requirments based on the group signature scheme introduced by Derler et al. [^1]
The scheme uses a
* NIZK scheme --> Schnorr's nizk protocol[^2]
* DS scheme --> BLS signatures [^3]
* PKE scheme --> ECIES [^4]

## Directory structure
* `demo`: An arduino demo application illustrating the CPA and CCA2 version of the RemoteID scheme in different scenarios :
  - `cca2`: cryptographic Adaptive chosen-ciphertext attack secure version
    -  setup, verify, open are performed on dekstop while sign phase is performed on drone.
      - `orginal`: without pre comp.
      - `pre-comp`: with pre comp.
  - `cpa`: Chosen-plaintext attack secure version
    -  setup, verify, open are performed on dekstop while sign phase is performed on drone.
      - `orginal`: without pre comp.
      - `pre-comp`: with pre comp.
* `include`: The headers required to run the demo and desktop version.
* `proverif`: A proverif proof proving that this satisfies the requirements set by the FAA.
* `src`: Code of the schemes used (nizk, ds, pke)
* `desktop`: Desktop version

[^1]: Fuchsbauer, G., Hanser, C., & Slamanig, D. (2019). Structure-Preserving Signatures on Equivalence Classes and Constant-Size Anonymous Credentials. Journal of Cryptology, 32(2), 498–546. https://doi.org/10.1007/s00145-018-9281-4
[^2]: Hao, F. (2017). Schnorr Non-interactive Zero-Knowledge Proof (Issue 8235, pp. 1–13). RFC Editor. https://doi.org/10.17487/RFC8235
[^3]: Boneh, D., Gorbunov, S., Wahby, R. S., Wee, H., Wood, C. A., & Zhang, Z. (2022). BLS Signatures (Issue draft-irtf-cfrg-bls-signature-05). Internet Engineering Task Force. https://datatracker.ietf.org/doc/draft-irtf-cfrg-bls-signature/05/
[^4]: Gayoso Martínez, V., Encinas, L. H., & Sánchez Ávila, C. (2010). A Survey of the Elliptic Curve Integrated Encryption Scheme. Journal of Computer Science and Engineering, 2(2), 7. http://sites.google.com/site/jcseuk/
