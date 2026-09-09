/*
	Giai ma 1 frame RF (24 bit -> data[3]/cmd[4]), dung chung cho ca 2 san pham.
	#include NGAY SAU "if(rfprocess){" trong main.c, sau khi da khai bao
	"u8 i,data[3],cmd[4]; u8 match=0;".

	LICH SU (de hieu vi sao khong con "xac nhan 2 frame" o day):
	  - A7-1.7/RL-1.1: "chot ngay 24 bit" -> hoc/bam remote nhanh nhat, nhung o
	    moi truong nhieu, nhieu ngau nhien du 24 "bit" co the bi hieu la remote
	    la va tu hoc nham vao bang.
	  - A7-1.8..CC-1.9/RL-1.3: them yeu cau 2 frame giong nhau moi cho hoc ->
	    chan nhieu, NHUNG hoc remote kho han (nhat la moi truong nhieu).
	  - CC-1.10/RL-1.4: noi long thanh "cua so khoan dung" - do dang.
	  - CC-1.11/RL-1.5 (ban nay): nguoi dung da test 4 ban tren phan cung va CHON
	    hanh vi hoc/doc remote cua 1.7 (de + nhay nhat). Vi vay o day BO hoan
	    toan buoc xac nhan - hoc ngay khi giai ma duoc 1 frame, dung nhu 1.7.
	    Danh doi: co the thi thoang tu hoc nham 1 "remote" nhieu vao bang; bu
	    lai da co chuc nang XOA REMOTE (them tu 1.9) de xoa cac muc rac do. Duong
	    DIEU KHIEN (mo/dong cua, toggle relay) van an toan vi doi hoi khop bang
	    da hoc san - nhieu gan nhu khong the tai tao dung 24 bit da hoc.
*/
data[0]  = rfdata[0]*128 +rfdata[1]*64+rfdata[2]*32+rfdata[3]*16;
data[0] += rfdata[4]*8 + rfdata[5]*4 + rfdata[6]*2 +rfdata[7];
data[1]  = rfdata[8]*128 +rfdata[9]*64+rfdata[10]*32+rfdata[11]*16;
data[1] += rfdata[12]*8 + rfdata[13]*4 + rfdata[14]*2 +rfdata[15];
if(pt2240){
	data[2] = rfdata[16]*8 + rfdata[17]*4 + rfdata[18]*2 +rfdata[19];
	cmd[1] = rfdata[21]; cmd[2] = rfdata[22]; cmd[3] = rfdata[23];
}
else{
	data[2] = 0;
	cmd[1] = rfdata[22]; cmd[2] = rfdata[18]; cmd[3] = rfdata[16];
}
cmd[0] = rfdata[20];
