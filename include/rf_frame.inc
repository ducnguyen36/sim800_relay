/*
	Giai ma 1 frame RF (24 bit -> data[3]/cmd[4]) + XAC NHAN LAP LAI, dung chung
	cho ca 2 san pham. #include NGAY SAU "if(rfprocess){" trong main.c, sau khi
	da khai bao "u8 i,data[3],cmd[4]; u8 match=0;".

	HOTFIX (sau A7-1.7/RL-1.1): fix "chot ngay 24 bit" da bo mat buoc doi sync
	ke tiep truoc day - buoc do vo tinh loc nhieu. O moi truong RF nhieu, chuoi
	nhieu ngau nhien cung du 24 "bit" se chot thanh frame va bi hieu la remote
	la -> tu hoc nham / co the kich hoat relay/cua gia.

	Fix dung: chi coi la ON DINH (khong phai nhieu) khi 2 FRAME LIEN TIEP giai
	ma ra DUNG CUNG 1 MA 3 byte. Remote that phat lien tuc khi giu/bam nen van
	nhanh; nhieu gan nhu khong bao gio lap lai dung 2 lan lien tiep -> loc sach.
	Dùng bien "rf_on_dinh" (bit) de gate ca hoc VA hanh dong o noi goi.
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

if(data[0]==rf_last_data[0] && data[1]==rf_last_data[1] && data[2]==rf_last_data[2]){
	if(rf_repeat<255) rf_repeat++;
}else{
	rf_repeat = 1;
	rf_last_data[0]=data[0]; rf_last_data[1]=data[1]; rf_last_data[2]=data[2];
}
rf_on_dinh = (rf_repeat>=2);
