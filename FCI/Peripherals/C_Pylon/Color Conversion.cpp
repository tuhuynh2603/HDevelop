///Original from Basler
void ConvertRawY8BGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pBuf+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pBuf+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pDst++;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }
    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pDst++;
  }

  memset(pBGR+(XSize*(YSize-1))*3,0,XSize*3);
}


void ConvertRawY8BGGREx(UINT32 XSize, UINT32 YSize, UINT8 *pBuf, UINT8 *pBGR, UINT8 ur, UINT8 ug, UINT8 ub)
{
	UINT8 *pR, *pB, *pG0, *pG1;
	UINT32 i, j;
	struct pdst {
		UINT8 B;
		UINT8 G;
		UINT8 R;
	};

	pdst* pDstCol = (struct pdst*)pBGR;
	pdst* pDstRow;

	int twoRows = XSize + XSize;
	
	for(i=0;i<YSize-1;i+=2) {
		pDstRow = pDstCol;

		pB = pBuf;
		pG0 = pB + 1;
		pG1 = pB + XSize;
		pR = pG1 + 1;
		for(j=0; j<XSize-1; j+=2) {
			pDstRow->B = (*pB * ub) >> 8;
			pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
			pDstRow->R = (*pR * ur) >>8;
			pDstRow++;
			pB+=2;
			pG1+=2;
			
			pDstRow->B = (*pB * ub) >> 8;
			pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
			pDstRow->R = (*pR * ur) >> 8;
			pDstRow++;
			pR+=2;
			pG0+=2;
		}

		pG1 = pBuf + XSize;
		pR = pG1 + 1;
		pB = pG1 + XSize;
		pG0 = pB + 1;
		
		for(j=0; j<XSize-1; j+=2) {
			pDstRow->B = (*pB * ub) >> 8;
			pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
			pDstRow->R = (*pR * ur) >>8;	
			pDstRow++;
			pB += 2;
			pG1 += 2;
			
			pDstRow->B = (*pB * ub) >> 8;
			pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
			pDstRow->R = (*pR * ur) >>8;
			pDstRow++;
			pR += 2;
			pG0 += 2;
		}

		pBuf += twoRows;
		pDstCol += twoRows;
	}
}

void ConvertRawY8BGGREx(UINT32 XSize, UINT32 YSize, UINT8 *pBuf, UINT8 *pBGR, double r, double g, double b)
{
	UINT8 *pR, *pB, *pG0, *pG1;
	UINT32 i, j;
	int tempr, tempg, tempb;
	struct pdst {
		UINT8 B;
		UINT8 G;
		UINT8 R;
	};

	int ub =(int)(b*1024);
	int ur = (int)(r*1024);
	pdst* pDstCol = (struct pdst*)pBGR;
	pdst* pDstRow;

	int twoRows = XSize + XSize;
	
	for(i=0;i<YSize-1;i+=2) {
		pDstRow = pDstCol;

		pB = pBuf;
		pG0 = pB + 1;
		pG1 = pB + XSize;
		pR = pG1 + 1;
		for(j=0; j<XSize-1; j+=2) {
			tempb = (*pB * ub)>>10;
			tempg = (*pG0+*pG1)>>1;
			tempr = (*pR * ur)>>10;
			pDstRow->B = tempb > 255?255:tempb;
			pDstRow->G = tempg > 255?255:tempg;
			pDstRow->R = tempr > 255?255:tempr;		
			pDstRow++;
			pB+=2;
			pG1+=2;
			
			tempb = (*pB * ub)>>10;
			tempg = (*pG0+*pG1)>>1;
			tempr = (*pR * ur)>>10;
			pDstRow->B = tempb > 255?255:tempb;
			pDstRow->G = tempg > 255?255:tempg;
			pDstRow->R = tempr > 255?255:tempr;
			pDstRow++;
			pR+=2;
			pG0+=2;
		}

		pG1 = pBuf + XSize;
		pR = pG1 + 1;
		pB = pG1 + XSize;
		pG0 = pB + 1;
		
		for(j=0; j<XSize-1; j+=2) {
			tempb = (*pB * ub)>>10;
			tempg = (*pG0+*pG1)>>1;
			tempr = (*pR * ur)>>10;
			pDstRow->B = tempb > 255?255:tempb;
			pDstRow->G = tempg > 255?255:tempg;
			pDstRow->R = tempr > 255?255:tempr;	
			pDstRow++;
			pB += 2;
			pG1 += 2;
			
			tempb = (*pB * ub)>>10;
			tempg = (*pG0+*pG1)>>1;
			tempr = (*pR * ur)>>10;
			pDstRow->B = tempb > 255?255:tempb;
			pDstRow->G = tempg > 255?255:tempg;
			pDstRow->R = tempr > 255?255:tempr;
			pDstRow++;
			pR += 2;
			pG0 += 2;
		}

		pBuf += twoRows;
		pDstCol += twoRows;
	}
}
