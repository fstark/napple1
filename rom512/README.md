# Example of using a 512K ROM card with bank switching

Bank 0:
    Entry point at 0x2000
    Boot code, copies trampoline to RAM at 300
    Trampoline code:
        2 entry points
            Build table
                Switch to BANK
                Reads the signature at 0x2100
                "FSAB" + ADRS + "NAME OF ROM" + 0x00
                If signature found, copies to 800+BANK*64
            Execute bank
                Switch to BANK
                Jump to address from 800+BANK*64
    Calls trampoline
    Display menu
    At selection, call execute bank

All banks needs compatible mapping:
    23456789....Ef (f is optional)
