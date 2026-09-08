/*
	Giai ma 1 frame RF (24 bit -> data[3]/cmd[4]) + XAC NHAN LAP LAI, dung chung
	cho ca 2 san pham. #include NGAY SAU "if(rfprocess){" trong main.c, sau khi
	da khai bao "u8 i,data[3],cmd[4]; u8 match=0;".

	HOTFIX (sau A7-1.7/RL-1.1, ban dau o A7-1.8/RL-1.2): fix "chot ngay 24 bit"
	da bo mat buoc doi sync ke tiep truoc day - buoc do vo tinh loc nhieu.
	O moi truong RF nhieu, chuoi nhieu ngau nhien cung du 24 "bit" se chot
	thanh frame va bi hieu la remote la -> tu hoc nham vao bang.

	Ban dau (A7-1.8/RL-1.2/CC-1.9/RL-1.3) doi hoi 2 FRAME LIEN TIEP TUYET DOI
	giong nhau - qua khat khe: chi 1 frame bi meo do nhieu xen giua 2 frame
	that (remote dang giu, phat lien tuc) la mat tien do, phai bat dau lai tu
	dau -> hoc kho, cham hon han A7-1.6/1.7.

	Ban nay (sau CC-1.9/RL-1.3): thay "lien tiep tuyet doi" bang "cua so khoan
	dung" - cho phep toi da RF_WIN_MAX frame KHAC (nhieu xen giua) truoc khi bo
	ung vien hien tai, thay vi reset ngay lap tuc. Remote that giu nut se phat
	lai dung ma trong vai frame ke tiep du co 1-2 frame bi nhieu xen giua ->
	van xac nhan nhanh (~bang 1.7). Nhieu thuan tuy (khong co remote that) gan
	nhu khong bao gio tu khop dung 3 byte 2 lan trong 1 cua so nho -> van loc
	sach nhu 1.8/1.9. Dùng bien "rf_on_dinh" (bit) de gate hoc (KHONG gate
	hanh dong binh thuong - hanh dong da duoc bao ve boi yeu cau khop bang).
*/
#define RF_WIN_MAX 4
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

if(data[0]==rf_last_data[0] && data[1]==rf_last_data[1] && data[2]==rf_last_data[2]){
	// Khop voi ung vien dang theo doi -> tang dem, lam moi cua so khoan dung.
	if(rf_repeat<255) rf_repeat++;
	rf_win = RF_WIN_MAX;
}else if(rf_win){
	// Frame la (nhieu xen giua) nhung ung vien cu van con trong cua so khoan
	// dung -> bo qua frame la nay, KHONG mat tien do cua ung vien dang theo doi.
	rf_win--;
}else{
	// Het cua so ma van chua khop lan 2 -> bo ung vien cu, bat dau lai voi
	// frame vua nhan nay lam ung vien moi.
	rf_repeat = 1;
	rf_last_data[0]=data[0]; rf_last_data[1]=data[1]; rf_last_data[2]=data[2];
	rf_win = RF_WIN_MAX;
}
rf_on_dinh = (rf_repeat>=2);
