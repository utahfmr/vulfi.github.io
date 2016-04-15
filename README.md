# VULFI - An LLVM based Fault Injection Framework
## Introduction
VULFI is an open-source instruction-level fault injection framework developed using LLVM compiler infrastructure. VULFI performs fault injection at LLVM's intermediate representation (IR) level. Any front-end compiler which generates LLVM IR can be targeted for fault injection using VULFI. VULFI has been extensively tested with C++ and ISPC languages. It is capable of injecting faults into both vector and scalar instructions. Please fill out the user form if you would like to try out VULFI.

Vulfi supports all major LLVM versions starting from LLVM version 3.2. The top level vulfi directory has a master folder which contains the latest source code of VULFI supporting latest LLVM version. The top level directory has additional folders starting with the suffix "LLVM_", each folder containing vulfi source code supporting the respective version of LLVM. For example, the folder LLVM_32 contains the vulfi source code supporting LLVM version 3.2. It is our intention to keep vulfi update supporting the latest LLVM [development branch] (http://llvm.org/svn/llvm-project/llvm/trunk) while also maintaining reasonable support for older versions of LLVM. For installation instructions, usage, and license information please refer to the individual VULFI directories.


# Cite our work

When you use VULFI in your research work, please cite our work using below citation. More information about the paper could be found [here.](http://www.cs.utah.edu/~vcsharma/pubs/dpdns2016-shrg.pdf)

```
@inproceedings{dpdns16vulfi,
  author = {Vishal Chandra Sharma, Ganesh Gopalakrishnan, Sriram Krishnamoorthy},
  title = {Towards Reseiliency Evaluation of Vector Programs},
  booktitle = {21st IEEE Workshop on Dependable Parallel, Distributed and Network-Centric Systems (DPDNS)},
  year = {2016},
}
```




# Code documentation
