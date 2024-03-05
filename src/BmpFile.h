/*----------------------------------------------------------------------+
|		BmpFile											 				|
|       Author:     DuanYanSong  2011/05/14								|
|            Ver 1.0													|
|       Copyright (c)2011, WHU RSGIS DPGrid Group						|
|	         All rights reserved.                                       |
|		ysduan@sohu.com													|
+----------------------------------------------------------------------*/
#ifndef BMPFILE_H_DUANYANSONG_2011_05_14_10_38_345678976543
#define BMPFILE_H_DUANYANSONG_2011_05_14_10_38_345678976543

class CBmpFile
{
public:
    CBmpFile(){	m_pImgDat=NULL; m_Cols=m_Rows=m_PxlBytes =0;
			    m_pBmpInfo   = new BYTE[ sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256 ];//内存分配
				memset( m_pBmpInfo,0,sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256 );//赋初始值
				RGBQUAD *pColorTab = (RGBQUAD *)(m_pBmpInfo+sizeof(BITMAPINFOHEADER));//*pColorTab指向第三部分的起始地址
				for( int i=0;i<256;i++ ){ pColorTab[i].rgbRed=pColorTab[i].rgbGreen=pColorTab[i].rgbBlue=i;} 
			}//获得灰度调色板
    virtual ~CBmpFile(){ if (m_pImgDat) delete []m_pImgDat; m_pImgDat=NULL; if( m_pBmpInfo ) delete []m_pBmpInfo; m_pBmpInfo=NULL; };//释放两个内存空间
	BYTE*   operator[](int nIndex){ return (m_pImgDat+nIndex*m_Cols); };//返回的是第nIndex行的起始地址
	const	CBmpFile& operator=(CBmpFile& bmpFile){//作用类似拷贝位图
				CreateBmp( bmpFile.m_Cols,bmpFile.m_Rows,bmpFile.m_PxlBytes);
				//拷贝两部分
				memcpy( m_pImgDat,bmpFile.m_pImgDat,m_Cols*m_Rows*m_PxlBytes );
				memcpy( m_pBmpInfo,bmpFile.m_pBmpInfo,sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256 );
				return (*this); 
			};
	
	BOOL	CreateBmp(int cols,int rows,int pixelPytes){//pixelPytes是1表示一个字节，是3表示三个字节
				if (m_pImgDat) delete []m_pImgDat;
				m_pImgDat = new BYTE[cols*rows*pixelPytes];
				memset( m_pImgDat,255,cols*rows*pixelPytes );
				m_Cols=cols; m_Rows=rows; m_PxlBytes =pixelPytes;
				return TRUE;
			};
    BOOL    Load4File( LPCSTR lpstrPathName ){//参数传递文件的名字
                BITMAPFILEHEADER  bmFileHdr; BITMAPINFOHEADER bmInfoHdr; DWORD rw;
                HANDLE hFile = ::CreateFile( lpstrPathName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );//实际上起到打开文件的作用
                if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
                ::ReadFile( hFile,&bmFileHdr,sizeof(bmFileHdr),&rw,NULL );// 读第一部分 // ??? &rw
                if ( bmFileHdr.bfType != ((WORD)('M'<<8)|'B') ){ ::CloseHandle(hFile); return FALSE; } //判断是否是位图
                ::ReadFile( hFile,&bmInfoHdr ,sizeof(bmInfoHdr ),&rw,NULL );//读第二部分
                if ( (bmInfoHdr.biBitCount!=8)&&(bmInfoHdr.biBitCount!=24) ){ ::CloseHandle(hFile); ::MessageBox( ::GetFocus(),"Just read 8 or 24 bits Bitmap File.","Error",MB_OK ); return FALSE; }
                
                m_Cols      = bmInfoHdr.biWidth;
                m_Rows      = bmInfoHdr.biHeight;
                m_PxlBytes  = int(bmInfoHdr.biBitCount/8);

				if ( m_PxlBytes==1 ) // 如果是灰度图像，读第三部分（彩色不用读）Read Color Table
					::ReadFile( hFile,m_pBmpInfo+sizeof(BITMAPINFOHEADER),256*sizeof(RGBQUAD),&rw,NULL );

                if ( m_pImgDat ) delete m_pImgDat; m_pImgDat = new BYTE[ m_Cols*m_Rows*m_PxlBytes +8 ];
                ::SetFilePointer( hFile,bmFileHdr.bfOffBits,NULL,FILE_BEGIN );// 设置文件指针到第四部分
                int rowBytes = ( ( bmInfoHdr.biWidth*(bmInfoHdr.biBitCount)+31)& ~31 )/8 ;
                for ( int i=0;i<m_Rows;i++ ) ::ReadFile( hFile,m_pImgDat+i*m_Cols*m_PxlBytes,rowBytes,&rw,NULL );//一行一行的读
                ::CloseHandle(hFile);
                return TRUE;
            };
	//参考该函数写raw转bmp
    BOOL    Save2File( LPCSTR lpstrPathName ){
				BITMAPFILEHEADER  bmFileHdr; BITMAPINFOHEADER bmInfoHdr;   DWORD rw;
				HANDLE hFile    = ::CreateFile( lpstrPathName,GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL );
				if ( !hFile && hFile==INVALID_HANDLE_VALUE ) return FALSE;

				//第二部分
				memset( &bmInfoHdr,0,sizeof(bmInfoHdr) );
				bmInfoHdr.biSize	  = sizeof(bmInfoHdr); // 结构体大小
				bmInfoHdr.biPlanes    = 1;
				bmInfoHdr.biWidth     = m_Cols      ;
				bmInfoHdr.biHeight    = m_Rows      ;
				bmInfoHdr.biBitCount  = m_PxlBytes*8; // 每一个像素比特数          
				
				//第一部分
				memset( &bmFileHdr,0,sizeof(bmFileHdr) );
				bmFileHdr.bfType      = ((WORD)('M'<<8)|'B');//第一个结构体变量赋初值"BM"
				bmFileHdr.bfOffBits   = sizeof(bmFileHdr)+sizeof(bmInfoHdr)+1024; // 偏移量（获取第四部分的起始地址）
				int rowBytes = ( (bmInfoHdr.biWidth*(bmInfoHdr.biBitCount)+31)& ~31 )/8 ;
				bmFileHdr.bfSize      = bmFileHdr.bfOffBits + rowBytes*bmInfoHdr.biHeight ;// 文件大小（=偏移量+高度*宽度*每行字节数）
			
				
                // 第三部分
				RGBQUAD pColTab[256]; 
				for ( int i=0;i<256;i++ ) pColTab[i].rgbRed=pColTab[i].rgbGreen=pColTab[i].rgbBlue=i;// 颜色表每一项赋初值

				//变量赋值后要写入文件
				::WriteFile( hFile,&bmFileHdr,sizeof(bmFileHdr),&rw,NULL ); // 写入第一部分
				::WriteFile( hFile,&bmInfoHdr,sizeof(bmInfoHdr),&rw,NULL ); // 写入第二部分               
				::WriteFile( hFile,pColTab,sizeof(pColTab ),&rw,NULL );     // 写入第三部分
				
				// 写入第四部分
				if ( rowBytes==m_Cols*m_PxlBytes )
					::WriteFile( hFile,m_pImgDat,bmFileHdr.bfSize-bmFileHdr.bfOffBits,&rw,NULL );
				else{
					for (int i=0;i<bmInfoHdr.biHeight-1;i++ )// 不相等则一行一行的写
					{
						::WriteFile( hFile,m_pImgDat+i*m_Cols*m_PxlBytes,rowBytes,&rw,NULL );
					    ::WriteFile( hFile,m_pImgDat+i*m_Cols*m_PxlBytes,m_Cols*m_PxlBytes,&rw,NULL );
					    ::WriteFile( hFile,m_pImgDat,rowBytes-m_Cols*m_PxlBytes,&rw,NULL );
					}
				}
				::CloseHandle(hFile);
				return TRUE;
			};
	void	Draw2DC( HDC hDC,int x,int y ){
				BITMAPINFO *pBmInf = (BITMAPINFO*)m_pBmpInfo;
				pBmInf->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
				pBmInf->bmiHeader.biCompression = BI_RGB;
				pBmInf->bmiHeader.biPlanes      = 1;
				pBmInf->bmiHeader.biBitCount    = m_PxlBytes*8;
				pBmInf->bmiHeader.biWidth       = m_Cols;
				pBmInf->bmiHeader.biHeight      = m_Rows; 
				
				BYTE *pBuf = m_pImgDat;
				int rowBytes = ( (pBmInf->bmiHeader.biWidth*(pBmInf->bmiHeader.biBitCount)+31)& ~31 )/8;
				if ( rowBytes!=m_Cols*m_PxlBytes ){
					pBuf = new BYTE[m_Rows*rowBytes];
					for (int i=0;i<m_Rows;i++ ) memcpy( pBuf+i*rowBytes,m_pImgDat+i*m_Cols*m_PxlBytes,m_Cols*m_PxlBytes );					
				}
				int oldMod = ::SetStretchBltMode( hDC,STRETCH_DELETESCANS );
				::StretchDIBits( hDC,
								 x,y,pBmInf->bmiHeader.biWidth,pBmInf->bmiHeader.biHeight,
								 0,0,pBmInf->bmiHeader.biWidth,pBmInf->bmiHeader.biHeight,
								 pBuf,(CONST BITMAPINFO *)pBmInf,
								 DIB_RGB_COLORS,SRCCOPY );
				::SetStretchBltMode( hDC,oldMod );

				if ( pBuf!=m_pImgDat ) delete []pBuf;
			};
public:
    BYTE*    m_pImgDat;
    int      m_Cols,m_Rows,m_PxlBytes;

	BYTE*    m_pBmpInfo;

};

#endif
