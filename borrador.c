/*

Breakdown of 0644:
0: No special modes (like setuid, setgid, or sticky bit).
6: Permissions for the owner (user).
4: Permissions for the group.
4: Permissions for others.
Detailed Breakdown:
First digit (6 for owner):

6 in octal is equivalent to 110 in binary.
110 means:
4 (100): Read permission.
2 (010): Write permission.
0 (000): No execute permission.
So, the owner has read and write permissions.
Second digit (4 for group):

4 in octal is equivalent to 100 in binary.
100 means:
4 (100): Read permission.
0 (000): No write permission.
0 (000): No execute permission.
So, the group has read permission only.
Third digit (4 for others):

This is identical to the group permissions.
Others have read permission only.
Summary of 0644 Permissions:
Owner: Read and write (rw-).
Group: Read-only (r--).
Others: Read-only (r--).

*/