# 283-assignment1

Assignment 1: Discovering VMX Features

- GCP VM setup:
    - creat an ubuntu instance on GCP UI.
    - To enable virtualization on the machine, did the following steps:
        - exported the properties of the VM into an YAML file
    			command: gcloud compute instances export assignment-283 \
              					--destination=assignment-283.yml \
              					--zone=us-west1-b
              					(VM name: assignment-283)
        - Edited the YAML file but adding the following:
    			advancedMachineFeatures:
      				enableNestedVirtualization: true
        - Updated the VM with the new properties by running the following:
   	 		command: gcloud compute instances update-from-file assignment-283 \
              					--source=/home/for-283/assignment-283.yml \
              					--most-disruptive-allowed-action=RESTART \
              					--zone=us-west1-b
              					(VM name: assignment-283 || YAML file path: /home/for-283/assignment-283.yml)
    - To verify if the nested virtualization is enabled we can check the REST response for the machine on GCP UI.
    
- Clone the git directory
- Run these commands to setup:
    - sudo apt install gcc make
    - sudo apt-get linux-headers-$(uname -r)
    - make
    - sudo insmod ./cmpe283-1.ko
- To view the output do: dmesg

- Code: https://github.com/chisunku/CMPE283-assignment1/blob/main/cmpe283-1.c
- Output: https://github.com/chisunku/CMPE283-assignment1/blob/main/output
- Tertiary procbased controls cannot be set because, the output we can see that under Primary Procbased Controls, the “Activate tertiary controls” cannot be set. 
