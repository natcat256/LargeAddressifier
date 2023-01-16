# LargeAddressifier
A utility for making Windows applications aware of large addresses.

For 32-bit applications, this will uplift the 2 GB memory limit to 4 GB.

# Usage
The utility is to be used from the command line as shown:
```
LargeAddressifier.exe <file>
```
Where ``<file>`` represents the path to the target application.

If successful, a modified copy of the application will be created with ".new" prepended to its file extension.
