<h1>Assignment 1: Discovering VMX Features</h1>

- Work done by Chinmayi Sunku:
    - Setup GCP instance. 
    - Enabled nested virtualization 
    - Modified the cmpe283-1.c file by adding information for primary proc-based, secondary proc-based and tertiary proc-based MSRs. Referred the SDM to get the information about the controls related to the MSRs.
    - Added the cmpe283-1.c file and the make file into the kernel.
    - Printed the buffer output.
    - Generated the output file but using dmesg > output
    - Commit the file .c to the repository along with the output file
    
- Work done by Ujwala Mote:
    - Configured VM on GCP.
    - Enabled nested virtualization
    - configured MSR to support entry, exit and tertiary proc-based controls by adding it into cmpe283-1.c file. Referred SDM for the same
    - Added the cmpe283-1.c file and the make file into the kernel.
    - Printed the buffer output.
    - Generated the output file but using dmesg > output

1) GCP VM setup:
    - Create an ubuntu instance on GCP UI.
    - Steps to enable virtualization on the machine:
        - exported the properties of the VM into a YAML file
    	    command: gcloud compute instances export assignment-283 \
                     --destination=assignment-283.yml \
              		 --zone=us-west1-b
            (VM name: assignment-283)
        - Edited the YAML file by adding the following:
    		Add these lines: 
                "advancedMachineFeatures:
      				enableNestedVirtualization: true"
        - Updated the VM with the new properties by running the following:
   	 		command: gcloud compute instances update-from-file assignment-283 \
              		 --source=/home/for-283/assignment-283.yml \
              		 --most-disruptive-allowed-action=RESTART \
              		 --zone=us-west1-b
            (VM name: assignment-283 || YAML file path: /home/for-283/assignment-283.yml)    
    - To verify if the nested virtualization is enabled we can check the REST response for the machine on GCP UI.

2) Clone the git directory: git clone https://github.com/chisunku/CMPE283-assignment1.git
3) Run these commands to setup:
    - sudo apt install gcc make
    - sudo apt-get linux-headers-$(uname -r)
    - make
    - sudo insmod ./cmpe283-1.ko
4) Command to view the output: dmesg

- Code: https://github.com/chisunku/CMPE283-assignment1/blob/main/cmpe283-1.c
- Output: https://github.com/chisunku/CMPE283-assignment1/blob/main/output

NOTE: Tertiary proc-based controls cannot be set because, in the output we can see that under Primary Procbased Controls, the “Activate tertiary controls” cannot be set. 

