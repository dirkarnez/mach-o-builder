#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>

const char *instructions =
    // Assembly instructions to print "Hello, World!" using printf
    "_main:\n"
    "    sub    $0x8, %rsp\n"
    "    lea    L_str(%rip), %rdi\n"
    "    xor    %rax, %rax\n"
    "    call   _printf\n"
    "    add    $0x8, %rsp\n"
    "    xor    %eax, %eax\n"
    "    ret\n"
    "L_str:\n"
    "    .asciz  \"Hello, World!\"\n";

int main() {
    // Create a new Mach-O binary
    char *binary = (char *)malloc(sizeof(struct mach_header_64) + sizeof(struct segment_command_64) * 2 + sizeof(struct symtab_command) + strlen(instructions));
    memset(binary, 0, sizeof(struct mach_header_64) + sizeof(struct segment_command_64) * 2 + sizeof(struct symtab_command) + strlen(instructions));

    // Set the Mach-O header fields
    struct mach_header_64 *header = (struct mach_header_64 *)binary;
    header->magic = MH_MAGIC_64;
    header->cputype = CPU_TYPE_X86_64;
    header->cpusubtype = CPU_SUBTYPE_X86_64_ALL;
    header->filetype = MH_EXECUTE;
    header->ncmds = 3;  // Three commands: LC_SEGMENT_64 (twice) and LC_SYMTAB
    header->sizeofcmds = sizeof(struct segment_command_64) * 2 + sizeof(struct symtab_command);
    header->flags = MH_DYLDLINK;

    // Create a __TEXT segment command
    struct segment_command_64 *textSegment = (struct segment_command_64 *)(binary + sizeof(struct mach_header_64));
    textSegment->cmd = LC_SEGMENT_64;
    textSegment->cmdsize = sizeof(struct segment_command_64) + strlen(instructions);
    strcpy(textSegment->segname, "__TEXT");
    textSegment->vmaddr = 0x1000;
    textSegment->vmsize = 0x1000;
    textSegment->fileoff = sizeof(struct mach_header_64) + sizeof(struct segment_command_64) * 2 + sizeof(struct symtab_command);
    textSegment->filesize = sizeof(struct segment_command_64) + strlen(instructions);
    textSegment->maxprot = VM_PROT_READ | VM_PROT_EXECUTE;
    textSegment->initprot = VM_PROT_READ | VM_PROT_EXECUTE;
    textSegment->nsects = 0;
    textSegment->flags = 0;

    // Copy the assembly instructions to the __TEXT section
    char *textSection = binary + textSegment->fileoff;
    strcpy(textSection, instructions);

    // Create a __DATA segment command
    struct segment_command_64 *dataSegment = (struct segment_command_64 *)(binary + sizeof(struct mach_header_64) + sizeof(struct segment_command_64));
    dataSegment->cmd = LC_SEGMENT_64;
    dataSegment->cmdsize = sizeof(struct segment_command_64);
    strcpy(dataSegment->segname, "__DATA");
    dataSegment->vmaddr = 0x2000;
    dataSegment->vmsize = 0x1000;
    dataSegment->fileoff = sizeof(struct mach_header_64) + sizeof(struct segment_command_64) * 2 + sizeof(struct symtab_command) + strlen(instructions);
    dataSegment->filesize = 0;
    dataSegment->maxprot = VM_PROT_READ | VM_PROT_WRITE;
    dataSegment->initprot = VM_PROT_READ | VM_PROT_WRITE;
    dataSegment->nsects = 0;
    dataSegment->flags = 0;

    // Create a symbol table command
    struct symtab_command *symtab = (struct symtab_command *)(binary + sizeof(struct mach_header_64) + sizeof(struct segment_command_64) * 2);
    symtab->cmd = LC_SYMTAB;
    symtab->cmdsize = sizeof(struct symtab_command);
    symtab->symoff = 0;
    symtab->nsyms = 0;
    symtab->stroff = 0;
    symtab->strsize = 0;

       // Write the binary to a file
    FILE *file = fopen("output", "wb");
    fwrite(binary, sizeof(struct mach_header_64) + sizeof(struct segment_command_64) * 2 + sizeof(struct symtab_command), 1, file);
    fclose(file);

    free(binary);

    printf("Mach-O binary created successfully.\n");

    return 0;
}
