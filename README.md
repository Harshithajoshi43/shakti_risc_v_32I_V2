#  Minimal RISC-V SoC using SHAKTI E-Class Core on Lattice FPGA

This project implements a **minimal System-on-Chip (SoC)** based on the **SHAKTI E-Class** RISC-V processor core. The goal is to build a lightweight, functional RISC-V SoC suitable for small, resource-constrained FPGAs like the Lattice iCE40UP5K.The Shakti RISC‑V 32I Core on VSD FPGA project brings together two vibrant open‑source ecosystems—India’s SHAKTI processor family and the VSDOpen FPGA community—to create a low‑cost, hands‑on RISC‑V SoC platform. At its heart lies the SHAKTI E‑Class RV32I core, a clean‑slate, five‑stage in‑order pipelined CPU design implementing the full RV32I integer instruction set. 

---

## Reproduction Steps

Follow these steps in order. Each command is in its own code block so it can be copied independently:

1. **Clone the repository**  
   ```bash
   git clone https://github.com/Harshithajoshi43/shakti_risc_v_32I_V2
This command downloads a full copy of this repository (all files, history, and folders) into a new local folder named `shakti_risc_v_32I`.

2. **Change into the shakthi_basic directory**

   ```
   cd shakti_risc_v_32I_V2
   ```
   
3. **Build the FPGA bitstream**

   ```
   make clean
   make all
   ```
This command synthesizes your Verilog into a netlist, places & routes it for the target FPGA, and produces the final bitstream (`.bin`) along with intermediate files (e.g. `.json`, `.asc`) in the `build/` directory.
   
4. **Flash the board**

   ```
   sudo make flash
   ```
This command takes the generated FPGA bitstream (`mkSoc.bin`) from the `build/` directory and programs it onto the connected FPGA board over USB/JTAG, so your design actually runs on the hardware.
   
---

## Steps used to generate firmware.hex

1. **Compile C code into ELF binary using your linker script**

```
riscv32-unknown-elf-gcc -Wall -O0 -ffreestanding -nostdlib -mabi=ilp32 -march=rv32i -o main.elf main.c -T linker.ld
```

2. **Convert ELF to raw binary**

```
riscv32-unknown-elf-objcopy -O binary main.elf main.bin
```

3. **Convert binary to hex (4 bytes per line, compatible with SHAKTI memory format)**

```
xxd -p -c 4 main.bin > firmware.hex
```

##  Project Objective

The objective of this project is to:

- Implement a working 32‑bit RISC‑V RV32I processor using the SHAKTI E‑Class core.  
- Integrate basic memory and UART I/O within a simplified SoC fabric.  
- Demonstrate UART output from compiled firmware running on the SHAKTI core.  
- Fit the entire SoC within the limitations of a low‑cost Lattice FPGA.  
- Serve as a learning platform for students and beginners in RISC‑V, FPGA, and SoC development.

---

##  SoC Architecture

The SoC consists of the following primary components:

1. **SHAKTI E‑Class CPU Core**  
   - A 5‑stage in‑order pipelined processor implementing the RV32I instruction set.  
   - Sourced from the SHAKTI processor family (IIT‑Madras).  
   - Communicates with peripherals via an AXI4‑Lite interconnect.

2. **Instruction Memory (BRAM)**  
   - A block RAM preloaded with a compiled firmware binary (`firmware.bin`).  
   - Serves as read‑only instruction memory for the CPU.

3. **UART Peripheral**  
   - A memory‑mapped UART transmitter.  
   - Mapped to address `0x9000_0000` in the CPU’s address space.  
   - Outputs characters serially from executing firmware.

4. **AXI4‑Lite Interconnect**  
   - A lightweight bus linking CPU, BRAM, and UART.  
   - Provides memory‑mapped access for instruction fetch and data I/O.

---


##  Project Overview

This project implements a fully functional, synthesizable RISC‑V system-on-chip (SoC) around SHAKTI’s E‑Class CPU core. The design is organized into clear, reusable modules:

- **CPU Core (`eclass`)**: A parameterized, five‑stage pipeline (IF → ID → EX → MEM → WB) with separate instruction and data buses.  
- **Instruction Memory (`BRAM`)**: On‑chip block RAM preloaded with `firmware.bin`. The fetch unit reads instructions synchronously, eliminating external memory dependencies.  
- **AXI4‑Lite Interconnect (`fabric`)**: A lightweight crossbar that arbitrates access between the CPU’s instruction fetch, data load/store, and memory‑mapped peripherals.  
- **UART Peripheral (`uart_user_ifc`)**: A simple, register‑based UART transmitter/receiver mapped at address `0x9000_0000`, supporting basic TX buffering via FIFOs.  
- **Clock/Reset Generation**: A centralized module that derives the global clock and synchronous reset (active‑low) signals for all subunits.  
- **Boot Loader & Interfaces**: A minimal boot module that initializes the program counter and offers JTAG/UART download paths for firmware.  
- **Debug & Simulation Hooks**: Dump interfaces (`io_dump_get`) and simulation monitors (`mv_end_simulation`) to facilitate post‑simulation state inspection and automated testbench control.

By modularizing each function—pipeline stages, ALU, CSR, register file, memory interfaces, FIFOs—this repository becomes an ideal learning sandbox. Newcomers can trace a single instruction’s journey through fetch, decode, execute, memory, and write‑back stages, while advanced users can experiment with custom peripherals or performance optimizations. 

---

## 🔍 System Block Diagram

![WhatsApp Image 2025-05-24 at 18 17 38_825da6d7](https://github.com/user-attachments/assets/0027b169-4ba1-4df1-b2c5-42eab3f37c16)


---

## Working Principle

1. **Power‑On & Reset**  
   - Upon power‑up, the **Clock/Reset Generator** asserts a synchronous, active‑low reset (`RST_N`) for a few clock cycles, ensuring all registers and FIFOs initialize to known states.  
   - The **Boot Loader** then deasserts reset and programs the CPU’s Program Counter (PC) with the start address of the instruction BRAM.

2. **Instruction Fetch (IF)**  
   - The CPU core drives the AXI4‑Lite instruction bus: `awvalid`/`arvalid` signals, `awaddr`/`araddr` pointers, and `rready` handshakes.  
   - The BRAM module returns 32‑bit instructions synchronously on each clock, feeding them into the IF stage FIFO.

3. **Instruction Decode (ID)**  
   - The fetched instruction is unpacked: opcode, register addresses, immediate fields.  
   - Two register‑read ports access the **Register File**, providing operand data for the execution stage.  
   - Control signals (branch, ALU opcode, CSR flags) are generated.

4. **Execute (EX)**  
   - The **ALU** performs arithmetic/logic operations (ADD, SUB, AND, OR, SHIFT, etc.), while the **CSR Unit** handles system and control‑status register instructions.  
   - Branch decisions are evaluated; if a branch is taken, the PC is updated accordingly via the interconnect.

5. **Memory Access (MEM)**  
   - For loads/stores, the data bus issues AXI4‑Lite read (`ar*`) or write (`aw*`, `w*`) transactions to the BRAM or peripherals.  
   - **Data FIFOs** ensure clock‑domain crossing safety and decouple back‑to‑back transactions.

6. **Write‑Back (WB)**  
   - Results from the ALU or loaded data are written back into the **Register File** on the rising edge of `CLK`.  
   - The core then fetches the next instruction, restarting the pipeline.

7. **UART I/O & Debug**  
   - Firmware writes ASCII values to the UART’s TX register; the **UART Peripheral** serializes and emits bits at the configured baud rate.  
   - Optional dump interface (`EN_io_dump_get` / `io_dump_get`) can snapshot internal state (registers, FIFOs) for offline analysis.  
   - A simulation monitor (`mv_end_simulation`) flags testbench completion, enabling fully automated CI integration.


---



##  Target Platform

- **FPGA Board:** VSDSquadron FPGA mini (Lattice iCE40 / Tang Nano 9K)  
- **Toolchain:** Yosys + NextPNR (iCE40)   
- **Language:** Verilog 2001  

---

##  Acknowledgments

- Core sourced from the [SHAKTI Processor Program](https://shakti.org.in/) by IIT‑Madras.   
- Thanks to the VSDOpen community for FPGA tooling guidance.
- Lattice Semiconductor for guidance

---

##  License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.



