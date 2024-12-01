//Ping detection (it detects incoming pings, but does not reply to them -only arp-)
//Write here your xbox ip address 
//(first 3 numbers should match your PC IP address, last one must be unique on your network)
#define MY_IP_ADDRESS "192.168.0.85"

#include <hal/xbox.h>
#include <hal/input.h>
#include <xboxkrnl/xboxkrnl.h>
#include <openxdk/debug.h>

#include "string.h"
#include "stdio.h"
#include <stdlib.h>

#include "memory.h"

#include "pktdrv.h"



struct _arp {
	//ethernet header (14 bytes)
	unsigned char	EthDst[6];		//0xFFFFFFFFFFFF=any (broadcast)
	unsigned char 	EthSrc[6];
	unsigned short 	ProtocolType;		//0x0608 (0x08 then 0x06)=IP ARP
	//arp header (2+2+1+1+2+6+4+6+4=28 bytes)
	unsigned short 	Hardware; 		//0x0100 (0x00 then 0x01)=Ethernet (10Mbps)
	unsigned short 	Protocol;		//0x0008 (0x08 then 0x00)=IP
	unsigned char 	HardwareAddrLen;	//6 (bytes)
	unsigned char 	ProtocolAddrLen;	//4 (bytes)
	unsigned short 	Operation;		//0x0100 (0x00 then 0x01)=Request 0x0200=Answer
	unsigned char	SenderHardwareAddr[6];
	unsigned char 	SenderProtocolAddr[4];	//Sender IP address
	unsigned char	TargetHardwareAddr[6];	//0 if not known yet
	unsigned char	TargetProtocolAddr[4];	//Target IP address
	};

struct _ip {
	//ethernet header (14 bytes)
	unsigned char	EthDst[6];		//0xFFFFFFFFFFFF=any (broadcast)
	unsigned char 	EthSrc[6];
	unsigned short 	ProtocolType;		//0x0008 (0x08 then 0x00)=IP
	//ip header (8 bytes)
	unsigned char	Version:4;		//4 bits, value 4	
	unsigned char	HeaderLength:4;		//4 bits, value 5 (means 20 bytes)
	unsigned char 	DifferentiatedServices;	//0
	unsigned short	TotalLength;		//0x2000 (0x00 then 0x20)=32 bytes
	unsigned short	Identifier;		//Variant
	unsigned short	FragmentationFlagOffset;//0
	unsigned char 	TimeToLive;		//128
	unsigned char 	Protocol;		//1 = ICMP (Internet Control Message Protocol)
	unsigned short	HeaderChecksum;		//Variant
	unsigned char 	SrcIPAddr[4];		//Sender IP address
	unsigned char	DstIPAddr[4];		//Target IP address
	//icmp sub-protocol
	unsigned char 	ICMPType;		//8 = Echo request
	unsigned char	ICMPCode;		//0
	unsigned short	Checksum;		//Variant
	unsigned short 	ID;			//Variant
	unsigned short	SequenceNumber;		//0 then increments with retries
	unsigned char  	ICMPDataArea[18];
	};



static unsigned int counter=0;
static unsigned int packetsize=0;
static unsigned char *packetbuffer=NULL;

unsigned int myipaddress;

void report(void)
{
	int i;
	unsigned char *p;
	struct _arp *parp;
	struct _ip *pip;

	debugPrint("Received packets : %ld\n",counter);
	debugPrint("Last received packet size : %d bytes\n",packetsize);
	if (packetsize<100)
	{

		p=packetbuffer;
		for(i=0;i<packetsize;i++)
		{
			if ((i==6)||(i==12)||(i==14)) debugPrint(" ");
			debugPrint("%02x",*p);
			p++;
		}
		debugPrint("\n");

		parp=(struct _arp *)packetbuffer;
		if (	(parp->ProtocolType==0x0608)&&
			(parp->Operation==0x100)	)
			debugPrint("It was an ARP request from %d.%d.%d.%d\n",
					parp->SenderProtocolAddr[0],
					parp->SenderProtocolAddr[1],
					parp->SenderProtocolAddr[2],
					parp->SenderProtocolAddr[3]);

		pip=(struct _ip *)packetbuffer;
		if (	(pip->ProtocolType==0x0008)&&
			(pip->Protocol==1)&&
			(pip->ICMPType==8)	)
			debugPrint("It was a Ping request from %d.%d.%d.%d!\nPktdrv worked!\n",
					pip->SrcIPAddr[0],
					pip->SrcIPAddr[1],
					pip->SrcIPAddr[2],
					pip->SrcIPAddr[3]);

	}
}

int Pktdrv_Callback(unsigned char *packetaddr, unsigned int size)
{
	//We are either called from a Dpc (if line uncommented in MyPktdrvDpc)
	//or from our own code (no need to be reentrant)

	counter++;
	packetsize=size;
	memcpy(packetbuffer,packetaddr,packetsize);

	return 1; //we declare we have taken the packet (reply 0 if you want keep it for later)
}

void process_last_packet(char *keymap, char *pmsex, char *pmsey)
{
	struct _ip *pip;
	struct _arp *parp;

	parp=(struct _arp *)packetbuffer;
	if (	(parp->ProtocolType==0x608)&&
		(parp->Operation==0x100)	)
	{
		//It's an ARP request

		unsigned char tmp[4];
		unsigned int ltmp;

		memcpy(tmp,parp->TargetProtocolAddr,4);
		ltmp=*((unsigned int *)tmp);

		//Are we the target?
		if (ltmp==myipaddress)
		{
			//Yes, build up an ARP response (invert src and dst)

			memcpy(parp->EthDst,parp->EthSrc,6);
			Pktdrv_GetEthernetAddr(parp->EthSrc);
			parp->Operation=0x200;
			memcpy(parp->TargetHardwareAddr,parp->SenderHardwareAddr,6);
			Pktdrv_GetEthernetAddr(parp->SenderHardwareAddr);
			memcpy(parp->TargetProtocolAddr,parp->SenderProtocolAddr,4);
			memcpy(parp->SenderProtocolAddr,tmp,4);

//			debugPrint("Answering to ARP request now!\n");

			//As a rule, always wait until number of packet not yet sent goes
			//below the number of buffers -ring- you are using to send them
			while (Pktdrv_GetQueuedTxPkts()>=1) { /*wait*/ };

			//In this sample we only send a reply to a request. It never waits.
			
			Pktdrv_SendPacket(packetbuffer,14+28);		
		}
	}

	pip=(struct _ip *)packetbuffer;
	if (	(pip->ProtocolType==0x0008)&&
		(pip->Protocol==1)&&
		(pip->ICMPType==8)	)
	{
		//It's a Ping request (an IP packet with a ICMP Echo request inside)

		unsigned char tmp[4];
		unsigned int ltmp;

		memcpy(tmp,pip->DstIPAddr,4);
		ltmp=*((unsigned int *)tmp);

		//Are we the target?
		if (ltmp==myipaddress)
		{
/*			debugPrint("It's a Ping request from %d.%d.%d.%d!\nPktdrv is working!\n",
				pip->SrcIPAddr[0],
				pip->SrcIPAddr[1],
				pip->SrcIPAddr[2],
				pip->SrcIPAddr[3]);
*/
			//Here we should build up an answer to the ping request
			//But we probably received it from inp2eth, so don't bother.

			//inp2eth extra data decoded here:
			if (packetsize>=sizeof(struct _ip)) //larger than a standard ping request?
			{
				int i,k;

				for(i=0;i<16;i++) 
					for(k=0;k<8;k++) 
						if (pip->ICMPDataArea[4+i]&(1<<(7-k)))
							keymap[i*8+k]=1;
						else
							keymap[i*8+k]=0;

				*pmsey=pip->ICMPDataArea[20];
				*pmsex=pip->ICMPDataArea[21];
			}
		}
	}
}


void detect_pkt_init(void)
{
	if (Pktdrv_Init())
		packetbuffer=(unsigned char *)MmAllocateContiguousMemoryEx(
			1520,
			0,		//lowest acceptable
			0xFFFFFFFF,	//highest acceptable
			0,  		//no need to align to specific boundaries multiple
			4);		//non cached, non ordered
	else
		packetbuffer=NULL;
}

void detect_myip(void)
{
	int i,v,n;
	char *p;
	unsigned char *p2;

	//convert IP address string into an unsigned int
	p=MY_IP_ADDRESS; //constant at top of this file
	p2=(unsigned char *)&myipaddress;
	for(i=0,n=0,v=0;i<strlen(p)+1;i++)
	{
		if ((p[i]>='0')&&(p[i]<='9'))
			v=v*10+p[i]-'0';
		else
			if ((p[i]=='.')||(p[i]=='\0'))
			{
				*p2=(unsigned char)(v&255);
				v=0;
				p2++;
				n++;
				if (n==4) break;
			}
	}
}

void detect_pkt(char *keymap,char *pmsex,char *pmsey)
{
	if (packetbuffer) 
	while (Pktdrv_ReceivePackets()) process_last_packet(keymap,pmsex,pmsey);
}

void detect_pkt_quit(void)
{
	if (packetbuffer)
	{
		Pktdrv_Quit();
		MmFreeContiguousMemory(packetbuffer);
	}
}

