#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <errno.h>

/* Define the defult destination MAC address */
#define DEFAULT_DEST_MAC0	0x00
#define DEFAULT_DEST_MAC1	0x03
#define DEFAULT_DEST_MAC2	0x7F
#define DEFAULT_DEST_MAC3	0xB0
#define DEFAULT_DEST_MAC4	0x20
#define DEFAULT_DEST_MAC5	0x20
 
#define DEFAULT_IF	        "wlan0"
#define BUF_SIZ	            2048	

int main(int argc, char *argv[])
{
	int sockfd1, sockfd2;
    int i;
	struct ifreq if_idx1;
	struct ifreq if_mac1;
    struct ifreq if_idx2;
	struct ifreq if_mac2;
	int tx_len1 = 0,Cnt;
    int tx_len2 = 0;
	char sendbuf1[BUF_SIZ];
    char sendbuf2[BUF_SIZ];
    unsigned int DstAddr1[6];
    unsigned int DstAddr2[6];
	struct ether_header *eh1 = (struct ether_header *) sendbuf1;
    struct ether_header *eh2 = (struct ether_header *) sendbuf2;
	struct iphdr *iph1 = (struct iphdr *) (sendbuf1 + sizeof(struct ether_header));
    struct iphdr *iph2 = (struct iphdr *) (sendbuf2 + sizeof(struct ether_header));
	struct sockaddr_ll socket_address1;
    struct sockaddr_ll socket_address2;
	char ifName1[IFNAMSIZ];
    char ifName2[IFNAMSIZ];
    if (argc == 1)
    {
        printf("Usage:   %s ifName DstMacAddr1 DstMacAddr2 NumOfPacketToSend\n",argv[0]);
        printf("Example: %s wlan0 00:7F:5D:3E:4A 00:7F:5D:3E:4A 100\n",argv[0]);
        exit(0);
    }
    if (argc > 1){
		strcpy(ifName1, argv[1]);
        strcpy(ifName2, argv[1]);
    }
	else{
		strcpy(ifName1, DEFAULT_IF);
        strcpy(ifName2, DEFAULT_IF);
    }

    //dst address seperated by :, example: 00:7F:5D:3E:4A
    if(argc > 2)
    {
        sscanf(argv[2],"%x:%x:%x:%x:%x:%x",&DstAddr1[0],&DstAddr1[1],&DstAddr1[2],&DstAddr1[3],&DstAddr1[4],&DstAddr1[5]);
        //printf("DstMacAddr: %02x:%02x:%02x:%02x:%02x:%02x\n",DstAddr[0],DstAddr[1],DstAddr[2],DstAddr[3],DstAddr[4],DstAddr[5]);
    }
    else if (argc > 3)
    {
        sscanf(argv[2],"%x:%x:%x:%x:%x:%x",&DstAddr2[0],&DstAddr2[1],&DstAddr2[2],&DstAddr2[3],&DstAddr2[4],&DstAddr2[5]);
    }
    if(argc > 4){
        Cnt = atoi(argv[4]);
    }
    else
        Cnt = 1;
    if ((sockfd1 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
	    perror("socket1 failed");
	}
    if ((sockfd2 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
	    perror("socket2 failed");
	}
    /* Get the index of the 1# interface to send on */
    memset(&if_idx1, 0, sizeof(struct ifreq));
	strncpy(if_idx1.ifr_name, ifName1, IFNAMSIZ-1);
	if (ioctl(sockfd1, SIOCGIFINDEX, &if_idx1) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the 1# interface to send on */
	memset(&if_mac1, 0, sizeof(struct ifreq));
	strncpy(if_mac1.ifr_name, ifName1, IFNAMSIZ-1);
	if (ioctl(sockfd1, SIOCGIFHWADDR, &if_mac1) < 0)
	    perror("SIOCGIFHWADDR");

    /* Get the index of the 2# interface to send on */
    memset(&if_idx2, 0, sizeof(struct ifreq));
	strncpy(if_idx2.ifr_name, ifName2, IFNAMSIZ-1);
	if (ioctl(sockfd2, SIOCGIFINDEX, &if_idx2) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the 2# interface to send on */
	memset(&if_mac2, 0, sizeof(struct ifreq));
	strncpy(if_mac2.ifr_name, ifName2, IFNAMSIZ-1);
	if (ioctl(sockfd2, SIOCGIFHWADDR, &if_mac2) < 0)
	    perror("SIOCGIFHWADDR");
    /* Construct the Ethernet header 1*/
	memset(sendbuf1, 0, BUF_SIZ);
	/* Ethernet header 1*/
	eh1->ether_shost[0] = ((uint8_t *)&if_mac1.ifr_hwaddr.sa_data)[0];
	eh1->ether_shost[1] = ((uint8_t *)&if_mac1.ifr_hwaddr.sa_data)[1];
	eh1->ether_shost[2] = ((uint8_t *)&if_mac1.ifr_hwaddr.sa_data)[2];
	eh1->ether_shost[3] = ((uint8_t *)&if_mac1.ifr_hwaddr.sa_data)[3];
	eh1->ether_shost[4] = ((uint8_t *)&if_mac1.ifr_hwaddr.sa_data)[4];
	eh1->ether_shost[5] = ((uint8_t *)&if_mac1.ifr_hwaddr.sa_data)[5];
	eh1->ether_dhost[0] = DstAddr1[0];
	eh1->ether_dhost[1] = DstAddr1[1];
	eh1->ether_dhost[2] = DstAddr1[2];
	eh1->ether_dhost[3] = DstAddr1[3];
	eh1->ether_dhost[4] = DstAddr1[4];
	eh1->ether_dhost[5] = DstAddr1[5];
    /* Construct the Ethernet header 2*/
	memset(sendbuf2, 0, BUF_SIZ);
	/* Ethernet header 2*/
	eh2->ether_shost[0] = ((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[0];
	eh2->ether_shost[1] = ((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[1];
	eh2->ether_shost[2] = ((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[2];
	eh2->ether_shost[3] = ((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[3];
	eh2->ether_shost[4] = ((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[4];
	eh2->ether_shost[5] = ((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[5];
	eh2->ether_dhost[0] = DstAddr2[0];
	eh2->ether_dhost[1] = DstAddr2[1];
	eh2->ether_dhost[2] = DstAddr2[2];
	eh2->ether_dhost[3] = DstAddr2[3];
	eh2->ether_dhost[4] = DstAddr2[4];
	eh2->ether_dhost[5] = DstAddr2[5];
    /* Ethertype field 1*/
	eh1->ether_type = htons(ETH_P_IP);
	tx_len1 += sizeof(struct ether_header);
 
	/* Packet data 1
     * We just set it to 0xaa you send arbitrary payload you like*/
    for(i=1;i<=1000;i++){
        
	    sendbuf1[tx_len1++] = 0xaa;
    } 
    printf("1# Packet Length is: %d,pkt_num is: %d\n",tx_len1,Cnt); 
        /* Ethertype field 2*/
	eh2->ether_type = htons(ETH_P_IP);
	tx_len2 += sizeof(struct ether_header);
 
	/* Packet data 2
     * We just set it to 0xaa you send arbitrary payload you like*/
    for(i=1;i<=1000;i++){
        
	    sendbuf2[tx_len2++] = 0xaa;
    } 
    printf("2# Packet Length is: %d,pkt_num is: %d\n",tx_len2,Cnt); 
    /* Index of the network device 1*/
	socket_address1.sll_ifindex = if_idx1.ifr_ifindex;
    /* RAW communication*/
    socket_address1.sll_family = PF_PACKET;    
    /* we don't use a protocoll above ethernet layer
     *   ->just use anything here*/
    socket_address1.sll_protocol = htons(ETH_P_IP);  
    
    /* ARP hardware identifier is ethernet*/
    socket_address1.sll_hatype   = ARPHRD_ETHER;
        
    /* target is another host*/
    socket_address1.sll_pkttype  = PACKET_OTHERHOST;
    
    /* address length*/
    socket_address1.sll_halen    = ETH_ALEN;
	/* Destination MAC */
	socket_address1.sll_addr[0] = DstAddr1[0];
	socket_address1.sll_addr[1] = DstAddr1[1];
	socket_address1.sll_addr[2] = DstAddr1[2];
	socket_address1.sll_addr[3] = DstAddr1[3];
	socket_address1.sll_addr[4] = DstAddr1[4];
	socket_address1.sll_addr[5] = DstAddr1[5];
    /* Index of the network device 2*/
	socket_address2.sll_ifindex = if_idx2.ifr_ifindex;
    /* RAW communication*/
    socket_address2.sll_family   = PF_PACKET;    
    /* we don't use a protocoll above ethernet layer
     *   ->just use anything here*/
    socket_address2.sll_protocol = htons(ETH_P_IP);  
    
    /* ARP hardware identifier is ethernet*/
    socket_address2.sll_hatype   = ARPHRD_ETHER;
        
    /* target is another host*/
    socket_address2.sll_pkttype  = PACKET_OTHERHOST;
    
    /* address length*/
    socket_address2.sll_halen    = ETH_ALEN;
	/* Destination MAC */
	socket_address2.sll_addr[0] = DstAddr2[0];
	socket_address2.sll_addr[1] = DstAddr2[1];
	socket_address2.sll_addr[2] = DstAddr2[2];
	socket_address2.sll_addr[3] = DstAddr2[3];
	socket_address2.sll_addr[4] = DstAddr2[4];
	socket_address2.sll_addr[5] = DstAddr2[5];
    /* Send packet */
    for(;Cnt>0;Cnt--)
    {
        /* you set the time interval between two transmitting packets 
         * for example, here we set it to 50 microseconds
         * set to 0 if you don't need it
         */
        if (usleep(200000) == -1){ // 0.2s send gap
            printf("sleep failed\n");
        }
        if (sendto(sockfd1, sendbuf1, tx_len1, 0, (struct sockaddr*)&socket_address1, sizeof(struct sockaddr_ll)) < 0 && sendto(sockfd2, sendbuf2, tx_len2, 0, (struct sockaddr*)&socket_address2, sizeof(struct sockaddr_ll)) < 0){
            printf("Send failed\n");
        }
    }
	
	return 0;