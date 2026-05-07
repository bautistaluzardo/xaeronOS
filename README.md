██╗  ██╗ █████╗ ███████╗██████╗  ██████╗ ███╗   ██╗
╚██╗██╔╝██╔══██╗██╔════╝██╔══██╗██╔═══██╗████╗  ██║
 ╚███╔╝ ███████║█████╗  ██████╔╝██║   ██║██╔██╗ ██║
 ██╔██╗ ██╔══██║██╔══╝  ██╔══██╗██║   ██║██║╚██╗██║
██╔╝ ██╗██║  ██║███████╗██║  ██║╚██████╔╝██║ ╚████║
╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

# xaeronOS

Un sistema operativo x86_64 experimental hecho desde cero como proyecto de aprendizaje de OSDev.

Actualmente el kernel:
- bootea con GRUB2
- entra en long mode (64-bit)
- usa paging
- maneja exceptions
- maneja IRQs
- recibe input del teclado PS/2
- renderiza texto en VGA text mode
- tiene una shell mínima funcional

---

# Features actuales

## Boot
- GRUB2 multiboot2
- Kernel ELF64
- Long mode
- Paging identity mapped
- GDT 64-bit

## CPU / Interrupts
- IDT
- Exceptions básicas
- IRQ remapping
- Keyboard IRQ (IRQ1)

## Drivers
- VGA text mode driver
- PS/2 keyboard driver

## Shell
- Prompt interactivo
- Input por teclado
- Parsing básico de comandos

Comandos actuales:

```text
help
clear
about
panic
```

## Filosofía del proyecto
### xaeronOS no busca ser práctico ni usable.
### El objetivo es aprender:

- arquitectura x86_64
- manejo de memoria
- interrupciones
- drivers
- diseño de kernels
- funcionamiento interno de sistemas operativos

### Todo el código está escrito desde cero sin libc ni frameworks externos.

## Referencias
- OSDev Wiki
- Intel SDM
- AMD64 Architecture Programmer’s Manual
- Las veces que se me rompio el kernel

## Licencia
MIT
