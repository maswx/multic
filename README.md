

### What is `multic`
	`multic` is 

### Build 

```shell
	git clone https://github.com/maswell/multic.git
	cd multic 
	mkdir build
	cd build
	cmake ..
	make
	./multic -h
```



### Example
```shell	
	multic                       #get help. the same as `multic -h` / `multic -help` / `multic --help`
	multic -v                    #version info and current version feature. the same as `multic --version` / `multic -V` / `multic -Version` / `multic -version`

	multic 0.1678901                    #multiplied by the constant 0.1678901. the data '0.1678901' precision is 16bit in default.
	multic 13.025234589 -f './out.txt'  #write the output to file. 
	
	multic 0.12345678 -wi '1 1 8' -wo '1 4 10'     #k

	multic '0.12 -1.4'            # calculate  0.12*IN1 - 1.4*IN2 .
	multic '-1e2 1e-1'            # calculate  
```



#### Parameter






