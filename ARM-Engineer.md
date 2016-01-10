# Embedded Sytems: ARM programming and Optimization

## Chap 1: The Linux ARM embedded platform

Today, ARM processors are sold as a reusable *macrocell*, which is a premade
design that is used as a module of a system-on-chip.

ARM is a “load-store” architecture.

ARM is a three-address architecture, meaning that a single instruction can
reference up to three register addresses.

16 user-accessible integer registers named **r0** through **r15**.
* **r14** is the link register, aka. **lr** or **LR**: is
updated by the hardware when executing a branch-and-link
instruction.
* **r15**, the *program counter*, aka. **pc** or **PC**: is
maintained by the hardware and used to control program flow.

### ARM Procedure Call Standard
When writing an assembly language routine—especially one that is embedded
in or callable from C code, comform to the *ARM Procedure Call Standard* (**APCS**):
* **r0** and **r1** are used for function return values.
* **r0** through **r3** (also called **a1**-**a4**, *argument registers*) are
used for passing arguments to functions.
* **r4** through **r11** (also called **v1**-**v8**, for *variable* registers) are
generally safe to use, except that some obscure compilers may use **r9** as
the *static base* register (also called **sb**, **SB**, or **v6**) and 
**r10** as the *stack limit register* (also called **sl**, **SL**, or **v7**).
* Compilers such as `gcc` use **r11** as the frame pointer (also called **fp**,
**FP**, or **v8**) to point to the base of the current *activation frame*. The
activation frame contains local subroutine data such as the local
variables.
* Register **r13** the *stack pointer* (also called **sp** or **SP**) is used to point to
the top of the activation stack.

### ARM assembly

### Status register
ARM v6/v7 maintains a status register called the **CPSR** (*current program
status register*) that holds four status bits, **negative (N)**, **zero (Z)**, **carry
(C)**, and **overflow (O)**. These bits can be used for conditional execution
of subsequent instructions.






