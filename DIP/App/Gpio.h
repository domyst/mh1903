
// PIOA define

#define PIOA  (*(volatile unsigned long *)(0x4001080C))
#define PINA  (*(volatile unsigned long *)(0x40010808))

#define pPA0_0  (*(volatile unsigned long *)(0x42210180)) =  0
#define pPA0_1  (*(volatile unsigned long *)(0x42210180)) =  1
#define pPA0_T  (*(volatile unsigned long *)(0x42210180)) ^= 1
#define pPA0_IN  (*(volatile unsigned long *)(0x42210100))

#define pPA1_0  (*(volatile unsigned long *)(0x42210184)) =  0
#define pPA1_1  (*(volatile unsigned long *)(0x42210184)) =  1
#define pPA1_T  (*(volatile unsigned long *)(0x42210184)) ^= 1
#define pPA1_IN  (*(volatile unsigned long *)(0x42210104))

#define pPA2_0  (*(volatile unsigned long *)(0x42210188)) =  0
#define pPA2_1  (*(volatile unsigned long *)(0x42210188)) =  1
#define pPA2_T  (*(volatile unsigned long *)(0x42210188)) ^= 1
#define pPA2_IN  (*(volatile unsigned long *)(0x42210108))

#define pPA3_0  (*(volatile unsigned long *)(0x4221018C)) =  0
#define pPA3_1  (*(volatile unsigned long *)(0x4221018C)) =  1
#define pPA3_T  (*(volatile unsigned long *)(0x4221018C)) ^= 1
#define pPA3_IN  (*(volatile unsigned long *)(0x4221010C))

#define pPA4_0  (*(volatile unsigned long *)(0x42210190)) =  0
#define pPA4_1  (*(volatile unsigned long *)(0x42210190)) =  1
#define pPA4_T  (*(volatile unsigned long *)(0x42210190)) ^= 1
#define pPA4_IN  (*(volatile unsigned long *)(0x42210110))

#define pPA5_0  (*(volatile unsigned long *)(0x42210194)) =  0
#define pPA5_1  (*(volatile unsigned long *)(0x42210194)) =  1
#define pPA5_T  (*(volatile unsigned long *)(0x42210194)) ^= 1
#define pPA5_IN  (*(volatile unsigned long *)(0x42210114))

#define pPA6_0  (*(volatile unsigned long *)(0x42210198)) =  0
#define pPA6_1  (*(volatile unsigned long *)(0x42210198)) =  1
#define pPA6_T  (*(volatile unsigned long *)(0x42210198)) ^= 1
#define pPA6_IN  (*(volatile unsigned long *)(0x42210118))

#define pPA7_0  (*(volatile unsigned long *)(0x4221019C)) =  0
#define pPA7_1  (*(volatile unsigned long *)(0x4221019C)) =  1
#define pPA7_T  (*(volatile unsigned long *)(0x4221019C)) ^= 1
#define pPA7_IN  (*(volatile unsigned long *)(0x4221011C))

#define pPA8_0  (*(volatile unsigned long *)(0x422101A0)) =  0
#define pPA8_1  (*(volatile unsigned long *)(0x422101A0)) =  1
#define pPA8_T  (*(volatile unsigned long *)(0x422101A0)) ^= 1
#define pPA8_IN  (*(volatile unsigned long *)(0x42210120))

#define pPA9_0  (*(volatile unsigned long *)(0x422101A4)) =  0
#define pPA9_1  (*(volatile unsigned long *)(0x422101A4)) =  1
#define pPA9_T  (*(volatile unsigned long *)(0x422101A4)) ^= 1
#define pPA9_IN  (*(volatile unsigned long *)(0x42210124))

#define pPA10_0  (*(volatile unsigned long *)(0x422101A8)) =  0
#define pPA10_1  (*(volatile unsigned long *)(0x422101A8)) =  1
#define pPA10_T  (*(volatile unsigned long *)(0x422101A8)) ^= 1
#define pPA10_IN  (*(volatile unsigned long *)(0x42210128))

#define pPA11_0  (*(volatile unsigned long *)(0x422101AC)) =  0
#define pPA11_1  (*(volatile unsigned long *)(0x422101AC)) =  1
#define pPA11_T  (*(volatile unsigned long *)(0x422101AC)) ^= 1
#define pPA11_IN  (*(volatile unsigned long *)(0x4221012C))

#define pPA12_0  (*(volatile unsigned long *)(0x422101B0)) =  0
#define pPA12_1  (*(volatile unsigned long *)(0x422101B0)) =  1
#define pPA12_T  (*(volatile unsigned long *)(0x422101B0)) ^= 1
#define pPA12_IN  (*(volatile unsigned long *)(0x42210130))

#define pPA13_0  (*(volatile unsigned long *)(0x422101B4)) =  0
#define pPA13_1  (*(volatile unsigned long *)(0x422101B4)) =  1
#define pPA13_T  (*(volatile unsigned long *)(0x422101B4)) ^= 1
#define pPA13_IN  (*(volatile unsigned long *)(0x42210134))

#define pPA14_0  (*(volatile unsigned long *)(0x422101B8)) =  0
#define pPA14_1  (*(volatile unsigned long *)(0x422101B8)) =  1
#define pPA14_T  (*(volatile unsigned long *)(0x422101B8)) ^= 1
#define pPA14_IN  (*(volatile unsigned long *)(0x42210138))

#define pPA15_0  (*(volatile unsigned long *)(0x422101BC)) =  0
#define pPA15_1  (*(volatile unsigned long *)(0x422101BC)) =  1
#define pPA15_T  (*(volatile unsigned long *)(0x422101BC)) ^= 1
#define pPA15_IN  (*(volatile unsigned long *)(0x4221013C))

// PIOB define

#define PIOB  (*(volatile unsigned long *)(0x40010C0C))
#define PINB  (*(volatile unsigned long *)(0x40010C08))

#define pPB0_0  (*(volatile unsigned long *)(0x42218180)) =  0
#define pPB0_1  (*(volatile unsigned long *)(0x42218180)) =  1
#define pPB0_T  (*(volatile unsigned long *)(0x42218180)) ^= 1
#define pPB0_IN  (*(volatile unsigned long *)(0x42218100))

#define pPB1_0  (*(volatile unsigned long *)(0x42218184)) =  0
#define pPB1_1  (*(volatile unsigned long *)(0x42218184)) =  1
#define pPB1_T  (*(volatile unsigned long *)(0x42218184)) ^= 1
#define pPB1_IN  (*(volatile unsigned long *)(0x42218104))

#define pPB2_0  (*(volatile unsigned long *)(0x42218188)) =  0
#define pPB2_1  (*(volatile unsigned long *)(0x42218188)) =  1
#define pPB2_T  (*(volatile unsigned long *)(0x42218188)) ^= 1
#define pPB2_IN  (*(volatile unsigned long *)(0x42218108))

#define pPB3_0  (*(volatile unsigned long *)(0x4221818C)) =  0
#define pPB3_1  (*(volatile unsigned long *)(0x4221818C)) =  1
#define pPB3_T  (*(volatile unsigned long *)(0x4221818C)) ^= 1
#define pPB3_IN  (*(volatile unsigned long *)(0x4221810C))

#define pPB4_0  (*(volatile unsigned long *)(0x42218190)) =  0
#define pPB4_1  (*(volatile unsigned long *)(0x42218190)) =  1
#define pPB4_T  (*(volatile unsigned long *)(0x42218190)) ^= 1
#define pPB4_IN  (*(volatile unsigned long *)(0x42218110))

#define pPB5_0  (*(volatile unsigned long *)(0x42218194)) =  0
#define pPB5_1  (*(volatile unsigned long *)(0x42218194)) =  1
#define pPB5_T  (*(volatile unsigned long *)(0x42218194)) ^= 1
#define pPB5_IN  (*(volatile unsigned long *)(0x42218114))

#define pPB6_0  (*(volatile unsigned long *)(0x42218198)) =  0
#define pPB6_1  (*(volatile unsigned long *)(0x42218198)) =  1
#define pPB6_T  (*(volatile unsigned long *)(0x42218198)) ^= 1
#define pPB6_IN  (*(volatile unsigned long *)(0x42218118))

#define pPB7_0  (*(volatile unsigned long *)(0x4221819C)) =  0
#define pPB7_1  (*(volatile unsigned long *)(0x4221819C)) =  1
#define pPB7_T  (*(volatile unsigned long *)(0x4221819C)) ^= 1
#define pPB7_IN  (*(volatile unsigned long *)(0x4221811C))

#define pPB8_0  (*(volatile unsigned long *)(0x422181A0)) =  0
#define pPB8_1  (*(volatile unsigned long *)(0x422181A0)) =  1
#define pPB8_T  (*(volatile unsigned long *)(0x422181A0)) ^= 1
#define pPB8_IN  (*(volatile unsigned long *)(0x42218120))

#define pPB9_0  (*(volatile unsigned long *)(0x422181A4)) =  0
#define pPB9_1  (*(volatile unsigned long *)(0x422181A4)) =  1
#define pPB9_T  (*(volatile unsigned long *)(0x422181A4)) ^= 1
#define pPB9_IN  (*(volatile unsigned long *)(0x42218124))

#define pPB10_0  (*(volatile unsigned long *)(0x422181A8)) =  0
#define pPB10_1  (*(volatile unsigned long *)(0x422181A8)) =  1
#define pPB10_T  (*(volatile unsigned long *)(0x422181A8)) ^= 1
#define pPB10_IN  (*(volatile unsigned long *)(0x42218128))

#define pPB11_0  (*(volatile unsigned long *)(0x422181AC)) =  0
#define pPB11_1  (*(volatile unsigned long *)(0x422181AC)) =  1
#define pPB11_T  (*(volatile unsigned long *)(0x422181AC)) ^= 1
#define pPB11_IN  (*(volatile unsigned long *)(0x4221812C))

#define pPB12_0  (*(volatile unsigned long *)(0x422181B0)) =  0
#define pPB12_1  (*(volatile unsigned long *)(0x422181B0)) =  1
#define pPB12_T  (*(volatile unsigned long *)(0x422181B0)) ^= 1
#define pPB12_IN  (*(volatile unsigned long *)(0x42218130))

#define pPB13_0  (*(volatile unsigned long *)(0x422181B4)) =  0
#define pPB13_1  (*(volatile unsigned long *)(0x422181B4)) =  1
#define pPB13_T  (*(volatile unsigned long *)(0x422181B4)) ^= 1
#define pPB13_IN  (*(volatile unsigned long *)(0x42218134))

#define pPB14_0  (*(volatile unsigned long *)(0x422181B8)) =  0
#define pPB14_1  (*(volatile unsigned long *)(0x422181B8)) =  1
#define pPB14_T  (*(volatile unsigned long *)(0x422181B8)) ^= 1
#define pPB14_IN  (*(volatile unsigned long *)(0x42218138))

#define pPB15_0  (*(volatile unsigned long *)(0x422181BC)) =  0
#define pPB15_1  (*(volatile unsigned long *)(0x422181BC)) =  1
#define pPB15_T  (*(volatile unsigned long *)(0x422181BC)) ^= 1
#define pPB15_IN  (*(volatile unsigned long *)(0x4221813C))

// PIOC define

#define PIOC  (*(volatile unsigned long *)(0x4001100C))
#define PINC  (*(volatile unsigned long *)(0x40011008))

#define pPC0_0  (*(volatile unsigned long *)(0x42220180)) =  0
#define pPC0_1  (*(volatile unsigned long *)(0x42220180)) =  1
#define pPC0_T  (*(volatile unsigned long *)(0x42220180)) ^= 1
#define pPC0_IN  (*(volatile unsigned long *)(0x42220100))

#define pPC1_0  (*(volatile unsigned long *)(0x42220184)) =  0
#define pPC1_1  (*(volatile unsigned long *)(0x42220184)) =  1
#define pPC1_T  (*(volatile unsigned long *)(0x42220184)) ^= 1
#define pPC1_IN  (*(volatile unsigned long *)(0x42220104))

#define pPC2_0  (*(volatile unsigned long *)(0x42220188)) =  0
#define pPC2_1  (*(volatile unsigned long *)(0x42220188)) =  1
#define pPC2_T  (*(volatile unsigned long *)(0x42220188)) ^= 1
#define pPC2_IN  (*(volatile unsigned long *)(0x42220108))

#define pPC3_0  (*(volatile unsigned long *)(0x4222018C)) =  0
#define pPC3_1  (*(volatile unsigned long *)(0x4222018C)) =  1
#define pPC3_T  (*(volatile unsigned long *)(0x4222018C)) ^= 1
#define pPC3_IN  (*(volatile unsigned long *)(0x4222010C))

#define pPC4_0  (*(volatile unsigned long *)(0x42220190)) =  0
#define pPC4_1  (*(volatile unsigned long *)(0x42220190)) =  1
#define pPC4_T  (*(volatile unsigned long *)(0x42220190)) ^= 1
#define pPC4_IN  (*(volatile unsigned long *)(0x42220110))

#define pPC5_0  (*(volatile unsigned long *)(0x42220194)) =  0
#define pPC5_1  (*(volatile unsigned long *)(0x42220194)) =  1
#define pPC5_T  (*(volatile unsigned long *)(0x42220194)) ^= 1
#define pPC5_IN  (*(volatile unsigned long *)(0x42220114))

#define pPC6_0  (*(volatile unsigned long *)(0x42220198)) =  0
#define pPC6_1  (*(volatile unsigned long *)(0x42220198)) =  1
#define pPC6_T  (*(volatile unsigned long *)(0x42220198)) ^= 1
#define pPC6_IN  (*(volatile unsigned long *)(0x42220118))

#define pPC7_0  (*(volatile unsigned long *)(0x4222019C)) =  0
#define pPC7_1  (*(volatile unsigned long *)(0x4222019C)) =  1
#define pPC7_T  (*(volatile unsigned long *)(0x4222019C)) ^= 1
#define pPC7_IN  (*(volatile unsigned long *)(0x4222011C))

#define pPC8_0  (*(volatile unsigned long *)(0x422201A0)) =  0
#define pPC8_1  (*(volatile unsigned long *)(0x422201A0)) =  1
#define pPC8_T  (*(volatile unsigned long *)(0x422201A0)) ^= 1
#define pPC8_IN  (*(volatile unsigned long *)(0x42220120))

#define pPC9_0  (*(volatile unsigned long *)(0x422201A4)) =  0
#define pPC9_1  (*(volatile unsigned long *)(0x422201A4)) =  1
#define pPC9_T  (*(volatile unsigned long *)(0x422201A4)) ^= 1
#define pPC9_IN  (*(volatile unsigned long *)(0x42220124))

#define pPC10_0  (*(volatile unsigned long *)(0x422201A8)) =  0
#define pPC10_1  (*(volatile unsigned long *)(0x422201A8)) =  1
#define pPC10_T  (*(volatile unsigned long *)(0x422201A8)) ^= 1
#define pPC10_IN  (*(volatile unsigned long *)(0x42220128))

#define pPC11_0  (*(volatile unsigned long *)(0x422201AC)) =  0
#define pPC11_1  (*(volatile unsigned long *)(0x422201AC)) =  1
#define pPC11_T  (*(volatile unsigned long *)(0x422201AC)) ^= 1
#define pPC11_IN  (*(volatile unsigned long *)(0x4222012C))

#define pPC12_0  (*(volatile unsigned long *)(0x422201B0)) =  0
#define pPC12_1  (*(volatile unsigned long *)(0x422201B0)) =  1
#define pPC12_T  (*(volatile unsigned long *)(0x422201B0)) ^= 1
#define pPC12_IN  (*(volatile unsigned long *)(0x42220130))

#define pPC13_0  (*(volatile unsigned long *)(0x422201B4)) =  0
#define pPC13_1  (*(volatile unsigned long *)(0x422201B4)) =  1
#define pPC13_T  (*(volatile unsigned long *)(0x422201B4)) ^= 1
#define pPC13_IN  (*(volatile unsigned long *)(0x42220134))

#define pPC14_0  (*(volatile unsigned long *)(0x422201B8)) =  0
#define pPC14_1  (*(volatile unsigned long *)(0x422201B8)) =  1
#define pPC14_T  (*(volatile unsigned long *)(0x422201B8)) ^= 1
#define pPC14_IN  (*(volatile unsigned long *)(0x42220138))

#define pPC15_0  (*(volatile unsigned long *)(0x422201BC)) =  0
#define pPC15_1  (*(volatile unsigned long *)(0x422201BC)) =  1
#define pPC15_T  (*(volatile unsigned long *)(0x422201BC)) ^= 1
#define pPC15_IN  (*(volatile unsigned long *)(0x4222013C))

// PIOD define

#define PIOD  (*(volatile unsigned long *)(0x4001140C))
#define PIND  (*(volatile unsigned long *)(0x40011408))

#define pPD0_0  (*(volatile unsigned long *)(0x42228180)) =  0
#define pPD0_1  (*(volatile unsigned long *)(0x42228180)) =  1
#define pPD0_T  (*(volatile unsigned long *)(0x42228180)) ^= 1
#define pPD0_IN  (*(volatile unsigned long *)(0x42228100))

#define pPD1_0  (*(volatile unsigned long *)(0x42228184)) =  0
#define pPD1_1  (*(volatile unsigned long *)(0x42228184)) =  1
#define pPD1_T  (*(volatile unsigned long *)(0x42228184)) ^= 1
#define pPD1_IN  (*(volatile unsigned long *)(0x42228104))

#define pPD2_0  (*(volatile unsigned long *)(0x42228188)) =  0
#define pPD2_1  (*(volatile unsigned long *)(0x42228188)) =  1
#define pPD2_T  (*(volatile unsigned long *)(0x42228188)) ^= 1
#define pPD2_IN  (*(volatile unsigned long *)(0x42228108))

#define pPD3_0  (*(volatile unsigned long *)(0x4222818C)) =  0
#define pPD3_1  (*(volatile unsigned long *)(0x4222818C)) =  1
#define pPD3_T  (*(volatile unsigned long *)(0x4222818C)) ^= 1
#define pPD3_IN  (*(volatile unsigned long *)(0x4222810C))

#define pPD4_0  (*(volatile unsigned long *)(0x42228190)) =  0
#define pPD4_1  (*(volatile unsigned long *)(0x42228190)) =  1
#define pPD4_T  (*(volatile unsigned long *)(0x42228190)) ^= 1
#define pPD4_IN  (*(volatile unsigned long *)(0x42228110))

#define pPD5_0  (*(volatile unsigned long *)(0x42228194)) =  0
#define pPD5_1  (*(volatile unsigned long *)(0x42228194)) =  1
#define pPD5_T  (*(volatile unsigned long *)(0x42228194)) ^= 1
#define pPD5_IN  (*(volatile unsigned long *)(0x42228114))

#define pPD6_0  (*(volatile unsigned long *)(0x42228198)) =  0
#define pPD6_1  (*(volatile unsigned long *)(0x42228198)) =  1
#define pPD6_T  (*(volatile unsigned long *)(0x42228198)) ^= 1
#define pPD6_IN  (*(volatile unsigned long *)(0x42228118))

#define pPD7_0  (*(volatile unsigned long *)(0x4222819C)) =  0
#define pPD7_1  (*(volatile unsigned long *)(0x4222819C)) =  1
#define pPD7_T  (*(volatile unsigned long *)(0x4222819C)) ^= 1
#define pPD7_IN  (*(volatile unsigned long *)(0x4222811C))

#define pPD8_0  (*(volatile unsigned long *)(0x422281A0)) =  0
#define pPD8_1  (*(volatile unsigned long *)(0x422281A0)) =  1
#define pPD8_T  (*(volatile unsigned long *)(0x422281A0)) ^= 1
#define pPD8_IN  (*(volatile unsigned long *)(0x42228120))

#define pPD9_0  (*(volatile unsigned long *)(0x422281A4)) =  0
#define pPD9_1  (*(volatile unsigned long *)(0x422281A4)) =  1
#define pPD9_T  (*(volatile unsigned long *)(0x422281A4)) ^= 1
#define pPD9_IN  (*(volatile unsigned long *)(0x42228124))

#define pPD10_0  (*(volatile unsigned long *)(0x422281A8)) =  0
#define pPD10_1  (*(volatile unsigned long *)(0x422281A8)) =  1
#define pPD10_T  (*(volatile unsigned long *)(0x422281A8)) ^= 1
#define pPD10_IN  (*(volatile unsigned long *)(0x42228128))

#define pPD11_0  (*(volatile unsigned long *)(0x422281AC)) =  0
#define pPD11_1  (*(volatile unsigned long *)(0x422281AC)) =  1
#define pPD11_T  (*(volatile unsigned long *)(0x422281AC)) ^= 1
#define pPD11_IN  (*(volatile unsigned long *)(0x4222812C))

#define pPD12_0  (*(volatile unsigned long *)(0x422281B0)) =  0
#define pPD12_1  (*(volatile unsigned long *)(0x422281B0)) =  1
#define pPD12_T  (*(volatile unsigned long *)(0x422281B0)) ^= 1
#define pPD12_IN  (*(volatile unsigned long *)(0x42228130))

#define pPD13_0  (*(volatile unsigned long *)(0x422281B4)) =  0
#define pPD13_1  (*(volatile unsigned long *)(0x422281B4)) =  1
#define pPD13_T  (*(volatile unsigned long *)(0x422281B4)) ^= 1
#define pPD13_IN  (*(volatile unsigned long *)(0x42228134))

#define pPD14_0  (*(volatile unsigned long *)(0x422281B8)) =  0
#define pPD14_1  (*(volatile unsigned long *)(0x422281B8)) =  1
#define pPD14_T  (*(volatile unsigned long *)(0x422281B8)) ^= 1
#define pPD14_IN  (*(volatile unsigned long *)(0x42228138))

#define pPD15_0  (*(volatile unsigned long *)(0x422281BC)) =  0
#define pPD15_1  (*(volatile unsigned long *)(0x422281BC)) =  1
#define pPD15_T  (*(volatile unsigned long *)(0x422281BC)) ^= 1
#define pPD15_IN  (*(volatile unsigned long *)(0x4222813C))

// PIOE define

#define PIOE  (*(volatile unsigned long *)(0x4001180C))
#define PINE  (*(volatile unsigned long *)(0x40011808))

#define pPE0_0  (*(volatile unsigned long *)(0x42230180)) =  0
#define pPE0_1  (*(volatile unsigned long *)(0x42230180)) =  1
#define pPE0_T  (*(volatile unsigned long *)(0x42230180)) ^= 1
#define pPE0_IN  (*(volatile unsigned long *)(0x42230100))

#define pPE1_0  (*(volatile unsigned long *)(0x42230184)) =  0
#define pPE1_1  (*(volatile unsigned long *)(0x42230184)) =  1
#define pPE1_T  (*(volatile unsigned long *)(0x42230184)) ^= 1
#define pPE1_IN  (*(volatile unsigned long *)(0x42230104))

#define pPE2_0  (*(volatile unsigned long *)(0x42230188)) =  0
#define pPE2_1  (*(volatile unsigned long *)(0x42230188)) =  1
#define pPE2_T  (*(volatile unsigned long *)(0x42230188)) ^= 1
#define pPE2_IN  (*(volatile unsigned long *)(0x42230108))

#define pPE3_0  (*(volatile unsigned long *)(0x4223018C)) =  0
#define pPE3_1  (*(volatile unsigned long *)(0x4223018C)) =  1
#define pPE3_T  (*(volatile unsigned long *)(0x4223018C)) ^= 1
#define pPE3_IN  (*(volatile unsigned long *)(0x4223010C))

#define pPE4_0  (*(volatile unsigned long *)(0x42230190)) =  0
#define pPE4_1  (*(volatile unsigned long *)(0x42230190)) =  1
#define pPE4_T  (*(volatile unsigned long *)(0x42230190)) ^= 1
#define pPE4_IN  (*(volatile unsigned long *)(0x42230110))

#define pPE5_0  (*(volatile unsigned long *)(0x42230194)) =  0
#define pPE5_1  (*(volatile unsigned long *)(0x42230194)) =  1
#define pPE5_T  (*(volatile unsigned long *)(0x42230194)) ^= 1
#define pPE5_IN  (*(volatile unsigned long *)(0x42230114))

#define pPE6_0  (*(volatile unsigned long *)(0x42230198)) =  0
#define pPE6_1  (*(volatile unsigned long *)(0x42230198)) =  1
#define pPE6_T  (*(volatile unsigned long *)(0x42230198)) ^= 1
#define pPE6_IN  (*(volatile unsigned long *)(0x42230118))

#define pPE7_0  (*(volatile unsigned long *)(0x4223019C)) =  0
#define pPE7_1  (*(volatile unsigned long *)(0x4223019C)) =  1
#define pPE7_T  (*(volatile unsigned long *)(0x4223019C)) ^= 1
#define pPE7_IN  (*(volatile unsigned long *)(0x4223011C))

#define pPE8_0  (*(volatile unsigned long *)(0x422301A0)) =  0
#define pPE8_1  (*(volatile unsigned long *)(0x422301A0)) =  1
#define pPE8_T  (*(volatile unsigned long *)(0x422301A0)) ^= 1
#define pPE8_IN  (*(volatile unsigned long *)(0x42230120))

#define pPE9_0  (*(volatile unsigned long *)(0x422301A4)) =  0
#define pPE9_1  (*(volatile unsigned long *)(0x422301A4)) =  1
#define pPE9_T  (*(volatile unsigned long *)(0x422301A4)) ^= 1
#define pPE9_IN  (*(volatile unsigned long *)(0x42230124))

#define pPE10_0  (*(volatile unsigned long *)(0x422301A8)) =  0
#define pPE10_1  (*(volatile unsigned long *)(0x422301A8)) =  1
#define pPE10_T  (*(volatile unsigned long *)(0x422301A8)) ^= 1
#define pPE10_IN  (*(volatile unsigned long *)(0x42230128))

#define pPE11_0  (*(volatile unsigned long *)(0x422301AC)) =  0
#define pPE11_1  (*(volatile unsigned long *)(0x422301AC)) =  1
#define pPE11_T  (*(volatile unsigned long *)(0x422301AC)) ^= 1
#define pPE11_IN  (*(volatile unsigned long *)(0x4223012C))

#define pPE12_0  (*(volatile unsigned long *)(0x422301B0)) =  0
#define pPE12_1  (*(volatile unsigned long *)(0x422301B0)) =  1
#define pPE12_T  (*(volatile unsigned long *)(0x422301B0)) ^= 1
#define pPE12_IN  (*(volatile unsigned long *)(0x42230130))

#define pPE13_0  (*(volatile unsigned long *)(0x422301B4)) =  0
#define pPE13_1  (*(volatile unsigned long *)(0x422301B4)) =  1
#define pPE13_T  (*(volatile unsigned long *)(0x422301B4)) ^= 1
#define pPE13_IN  (*(volatile unsigned long *)(0x42230134))

#define pPE14_0  (*(volatile unsigned long *)(0x422301B8)) =  0
#define pPE14_1  (*(volatile unsigned long *)(0x422301B8)) =  1
#define pPE14_T  (*(volatile unsigned long *)(0x422301B8)) ^= 1
#define pPE14_IN  (*(volatile unsigned long *)(0x42230138))

#define pPE15_0  (*(volatile unsigned long *)(0x422301BC)) =  0
#define pPE15_1  (*(volatile unsigned long *)(0x422301BC)) =  1
#define pPE15_T  (*(volatile unsigned long *)(0x422301BC)) ^= 1
#define pPE15_IN  (*(volatile unsigned long *)(0x4223013C))

// PIOF define
/*
#define PIOF  (*(volatile unsigned long *)(0x40011C0C))
#define PINF  (*(volatile unsigned long *)(0x40011C08))

#define pPF0_0  (*(volatile unsigned long *)(0x42238180)) =  0
#define pPF0_1  (*(volatile unsigned long *)(0x42238180)) =  1
#define pPF0_T  (*(volatile unsigned long *)(0x42238180)) ^= 1
#define pPF0_IN  (*(volatile unsigned long *)(0x42238100))

#define pPF1_0  (*(volatile unsigned long *)(0x42238184)) =  0
#define pPF1_1  (*(volatile unsigned long *)(0x42238184)) =  1
#define pPF1_T  (*(volatile unsigned long *)(0x42238184)) ^= 1
#define pPF1_IN  (*(volatile unsigned long *)(0x42238104))

#define pPF2_0  (*(volatile unsigned long *)(0x42238188)) =  0
#define pPF2_1  (*(volatile unsigned long *)(0x42238188)) =  1
#define pPF2_T  (*(volatile unsigned long *)(0x42238188)) ^= 1
#define pPF2_IN  (*(volatile unsigned long *)(0x42238108))

#define pPF3_0  (*(volatile unsigned long *)(0x4223818C)) =  0
#define pPF3_1  (*(volatile unsigned long *)(0x4223818C)) =  1
#define pPF3_T  (*(volatile unsigned long *)(0x4223818C)) ^= 1
#define pPF3_IN  (*(volatile unsigned long *)(0x4223810C))

#define pPF4_0  (*(volatile unsigned long *)(0x42238190)) =  0
#define pPF4_1  (*(volatile unsigned long *)(0x42238190)) =  1
#define pPF4_T  (*(volatile unsigned long *)(0x42238190)) ^= 1
#define pPF4_IN  (*(volatile unsigned long *)(0x42238110))

#define pPF5_0  (*(volatile unsigned long *)(0x42238194)) =  0
#define pPF5_1  (*(volatile unsigned long *)(0x42238194)) =  1
#define pPF5_T  (*(volatile unsigned long *)(0x42238194)) ^= 1
#define pPF5_IN  (*(volatile unsigned long *)(0x42238114))

#define pPF6_0  (*(volatile unsigned long *)(0x42238198)) =  0
#define pPF6_1  (*(volatile unsigned long *)(0x42238198)) =  1
#define pPF6_T  (*(volatile unsigned long *)(0x42238198)) ^= 1
#define pPF6_IN  (*(volatile unsigned long *)(0x42238118))

#define pPF7_0  (*(volatile unsigned long *)(0x4223819C)) =  0
#define pPF7_1  (*(volatile unsigned long *)(0x4223819C)) =  1
#define pPF7_T  (*(volatile unsigned long *)(0x4223819C)) ^= 1
#define pPF7_IN  (*(volatile unsigned long *)(0x4223811C))

#define pPF8_0  (*(volatile unsigned long *)(0x422381A0)) =  0
#define pPF8_1  (*(volatile unsigned long *)(0x422381A0)) =  1
#define pPF8_T  (*(volatile unsigned long *)(0x422381A0)) ^= 1
#define pPF8_IN  (*(volatile unsigned long *)(0x42238120))

#define pPF9_0  (*(volatile unsigned long *)(0x422381A4)) =  0
#define pPF9_1  (*(volatile unsigned long *)(0x422381A4)) =  1
#define pPF9_T  (*(volatile unsigned long *)(0x422381A4)) ^= 1
#define pPF9_IN  (*(volatile unsigned long *)(0x42238124))

#define pPF10_0  (*(volatile unsigned long *)(0x422381A8)) =  0
#define pPF10_1  (*(volatile unsigned long *)(0x422381A8)) =  1
#define pPF10_T  (*(volatile unsigned long *)(0x422381A8)) ^= 1
#define pPF10_IN  (*(volatile unsigned long *)(0x42238128))

#define pPF11_0  (*(volatile unsigned long *)(0x422381AC)) =  0
#define pPF11_1  (*(volatile unsigned long *)(0x422381AC)) =  1
#define pPF11_T  (*(volatile unsigned long *)(0x422381AC)) ^= 1
#define pPF11_IN  (*(volatile unsigned long *)(0x4223812C))

#define pPF12_0  (*(volatile unsigned long *)(0x422381B0)) =  0
#define pPF12_1  (*(volatile unsigned long *)(0x422381B0)) =  1
#define pPF12_T  (*(volatile unsigned long *)(0x422381B0)) ^= 1
#define pPF12_IN  (*(volatile unsigned long *)(0x42238130))

#define pPF13_0  (*(volatile unsigned long *)(0x422381B4)) =  0
#define pPF13_1  (*(volatile unsigned long *)(0x422381B4)) =  1
#define pPF13_T  (*(volatile unsigned long *)(0x422381B4)) ^= 1
#define pPF13_IN  (*(volatile unsigned long *)(0x42238134))

#define pPF14_0  (*(volatile unsigned long *)(0x422381B8)) =  0
#define pPF14_1  (*(volatile unsigned long *)(0x422381B8)) =  1
#define pPF14_T  (*(volatile unsigned long *)(0x422381B8)) ^= 1
#define pPF14_IN  (*(volatile unsigned long *)(0x42238138))

#define pPF15_0  (*(volatile unsigned long *)(0x422381BC)) =  0
#define pPF15_1  (*(volatile unsigned long *)(0x422381BC)) =  1
#define pPF15_T  (*(volatile unsigned long *)(0x422381BC)) ^= 1
#define pPF15_IN  (*(volatile unsigned long *)(0x4223813C))

// PIOG define

#define PIOG  (*(volatile unsigned long *)(0x4001200C))
#define PING  (*(volatile unsigned long *)(0x40012008))

#define pPG0_0  (*(volatile unsigned long *)(0x42240180)) =  0
#define pPG0_1  (*(volatile unsigned long *)(0x42240180)) =  1
#define pPG0_T  (*(volatile unsigned long *)(0x42240180)) ^= 1
#define pPG0_IN  (*(volatile unsigned long *)(0x42240100))

#define pPG1_0  (*(volatile unsigned long *)(0x42240184)) =  0
#define pPG1_1  (*(volatile unsigned long *)(0x42240184)) =  1
#define pPG1_T  (*(volatile unsigned long *)(0x42240184)) ^= 1
#define pPG1_IN  (*(volatile unsigned long *)(0x42240104))

#define pPG2_0  (*(volatile unsigned long *)(0x42240188)) =  0
#define pPG2_1  (*(volatile unsigned long *)(0x42240188)) =  1
#define pPG2_T  (*(volatile unsigned long *)(0x42240188)) ^= 1
#define pPG2_IN  (*(volatile unsigned long *)(0x42240108))

#define pPG3_0  (*(volatile unsigned long *)(0x4224018C)) =  0
#define pPG3_1  (*(volatile unsigned long *)(0x4224018C)) =  1
#define pPG3_T  (*(volatile unsigned long *)(0x4224018C)) ^= 1
#define pPG3_IN  (*(volatile unsigned long *)(0x4224010C))

#define pPG4_0  (*(volatile unsigned long *)(0x42240190)) =  0
#define pPG4_1  (*(volatile unsigned long *)(0x42240190)) =  1
#define pPG4_T  (*(volatile unsigned long *)(0x42240190)) ^= 1
#define pPG4_IN  (*(volatile unsigned long *)(0x42240110))

#define pPG5_0  (*(volatile unsigned long *)(0x42240194)) =  0
#define pPG5_1  (*(volatile unsigned long *)(0x42240194)) =  1
#define pPG5_T  (*(volatile unsigned long *)(0x42240194)) ^= 1
#define pPG5_IN  (*(volatile unsigned long *)(0x42240114))

#define pPG6_0  (*(volatile unsigned long *)(0x42240198)) =  0
#define pPG6_1  (*(volatile unsigned long *)(0x42240198)) =  1
#define pPG6_T  (*(volatile unsigned long *)(0x42240198)) ^= 1
#define pPG6_IN  (*(volatile unsigned long *)(0x42240118))

#define pPG7_0  (*(volatile unsigned long *)(0x4224019C)) =  0
#define pPG7_1  (*(volatile unsigned long *)(0x4224019C)) =  1
#define pPG7_T  (*(volatile unsigned long *)(0x4224019C)) ^= 1
#define pPG7_IN  (*(volatile unsigned long *)(0x4224011C))

#define pPG8_0  (*(volatile unsigned long *)(0x422401A0)) =  0
#define pPG8_1  (*(volatile unsigned long *)(0x422401A0)) =  1
#define pPG8_T  (*(volatile unsigned long *)(0x422401A0)) ^= 1
#define pPG8_IN  (*(volatile unsigned long *)(0x42240120))

#define pPG9_0  (*(volatile unsigned long *)(0x422401A4)) =  0
#define pPG9_1  (*(volatile unsigned long *)(0x422401A4)) =  1
#define pPG9_T  (*(volatile unsigned long *)(0x422401A4)) ^= 1
#define pPG9_IN  (*(volatile unsigned long *)(0x42240124))

#define pPG10_0  (*(volatile unsigned long *)(0x422401A8)) =  0
#define pPG10_1  (*(volatile unsigned long *)(0x422401A8)) =  1
#define pPG10_T  (*(volatile unsigned long *)(0x422401A8)) ^= 1
#define pPG10_IN  (*(volatile unsigned long *)(0x42240128))

#define pPG11_0  (*(volatile unsigned long *)(0x422401AC)) =  0
#define pPG11_1  (*(volatile unsigned long *)(0x422401AC)) =  1
#define pPG11_T  (*(volatile unsigned long *)(0x422401AC)) ^= 1
#define pPG11_IN  (*(volatile unsigned long *)(0x4224012C))

#define pPG12_0  (*(volatile unsigned long *)(0x422401B0)) =  0
#define pPG12_1  (*(volatile unsigned long *)(0x422401B0)) =  1
#define pPG12_T  (*(volatile unsigned long *)(0x422401B0)) ^= 1
#define pPG12_IN  (*(volatile unsigned long *)(0x42240130))

#define pPG13_0  (*(volatile unsigned long *)(0x422401B4)) =  0
#define pPG13_1  (*(volatile unsigned long *)(0x422401B4)) =  1
#define pPG13_T  (*(volatile unsigned long *)(0x422401B4)) ^= 1
#define pPG13_IN  (*(volatile unsigned long *)(0x42240134))

#define pPG14_0  (*(volatile unsigned long *)(0x422401B8)) =  0
#define pPG14_1  (*(volatile unsigned long *)(0x422401B8)) =  1
#define pPG14_T  (*(volatile unsigned long *)(0x422401B8)) ^= 1
#define pPG14_IN  (*(volatile unsigned long *)(0x42240138))

#define pPG15_0  (*(volatile unsigned long *)(0x422401BC)) =  0
#define pPG15_1  (*(volatile unsigned long *)(0x422401BC)) =  1
#define pPG15_T  (*(volatile unsigned long *)(0x422401BC)) ^= 1
#define pPG15_IN  (*(volatile unsigned long *)(0x4224013C))

*/

