Generate AMD BIOS blob for signing and add BIOS blob signature.

Extract the BIOS directory table and BIOS binary.
```
amdsigtool --rom coreboot.rom --bios-dir bios.dir --bios-bin bios.bin
```

Sign the BIOS binary concatenated with the BIOS directory table.
```
cat bios.bin bios.dir > bios-blob.bin
openssl dgst -binary -sign key.pem -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-2 -out sig.bin bios-blob.bin
xxd -p -c 1 sig.bin | tac | xxd -p -r > sig.bin.rev
```

Insert the BIOS signature into the BIOS directory table.
```
amdsigtool --rom coreboot.rom --update --bios-sig sig.bin.rev
```
