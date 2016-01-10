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


### Status register
ARM v6/v7 maintains a status register called the **CPSR** (*current program
status register*) that holds four status bits, **negative (N)**, **zero (Z)**, **carry
(C)**, and **overflow (O)**. These bits can be used for conditional execution
of subsequent instructions.
* The bits are set according to the most recently executed ALU instruction that
includes the special “`s`” suffix. For example, the “`adds`” instruction will
modify the status bits but the “`add`” instruction will not.

Nearly all ARM instructions can include an optional *condition code*. 
For example, the ldreq instruction will only execute if the Z-bit in the CPSR
is set, which will be the case if the most recent computational instruction
resulted in a result of zero:
```
subs r2,r2,#1
streq r3, [r0] 
```
…will decrement register **r2** and store **r3** only if the new value of **r2**
is zero.

The compare (`cmp`) instruction can be used to set the status bits without any
other side effect:
```
cmp r2,r3
streq r4, [r0]
```
…will store register **r4** only if the contents of registers **r2** and **r3**
are equal.

When combining the condition code and the “`s`” suffix, the condition code
comes first, for example: `addeqs r0,r0,r1`







### Memory addressing mode

![addressing_mode](https://cloud.githubusercontent.com/assets/14265605/12220081/7a3879e8-b724-11e5-9830-3960b06e205d.png)


### GNU ARM assembler





