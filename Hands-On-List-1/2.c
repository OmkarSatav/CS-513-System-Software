/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a simple program to execute in an infinite loop at the background. Go to /proc directory and identify all the process related information 
 * in the corresponding proc directory.
 * */


int main(void){
	for(1;1;1){} // Infinite loop
	return 0;
}


/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment2$ ./a.out &
[1] 26869
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment2$ stat /proc/26869
  File: /proc/26869
  Size: 0         	Blocks: 0          IO Block: 1024   directory
Device: 0,22	Inode: 233272      Links: 9
Access: (0555/dr-xr-xr-x)  Uid: ( 1000/   omkar)   Gid: ( 1000/   omkar)
Access: 2024-08-29 19:38:42.622986076 +0530
Modify: 2024-08-29 19:38:42.622986076 +0530
Change: 2024-08-29 19:38:42.622986076 +0530
 Birth: -
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment2$ cat /proc/26869/stat
26869 (a.out) R 26790 26869 26790 34816 27121 4194304 116 0 1 0 12252 1 0 0 20 0 1 0 2156176 2543616 224 18446744073709551615 94394308157440 94394308157761 140734529014672 0 0 0 0 0 0 0 0 0 17 6 0 0 0 0 0 94394308169200 94394308169744 94394323566592 140734529020463 140734529020471 140734529020471 140734529023984 0
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment2$ 


*/