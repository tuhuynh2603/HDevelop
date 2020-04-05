#pragma once
#include "PictureCtrl.h"

#define MAX_NO_DISPLAY		12
#define IMAGE_SETS			4

// CImageSelectionDlg dialog

//struct CImageHistory
//{
//	CImageBuffer m_StorageBuffer;	
//};

//struct CImageHistoryZoom
//{
//	CImageBuffer m_StorageBufferZoom;
//};

enum DefectInfo
{
	NOT_INSPECTED = -2,
	UNKNOWN_DEFECT = -1,
//	GOOD_UNIT = 0,
	ERROR_PROCESS = -99
};

struct GroupInfo
{
	GroupInfo()
		: id(0)
		, name("unknown")
	{ };

	GroupInfo(const std::string & aName, int anId)
		: id(anId)
		, name(aName)
	{ };

	int id;
	std::string name;
};

struct ItemInfo
{
	ItemInfo()
		: description("no description")
		, code("unknown code")
		, id(1)
		, groupId(0)
		, color(RGB(128, 128, 128))
		, good(false)
	{
		brush.CreateSolidBrush(color);
	};

	ItemInfo(const std::string & aDescription, const std::string & aCode, int anId, int aGroupId, COLORREF aColor, bool aGood)
		: description(aDescription)
		, code(aCode)
		, id(anId)
		, groupId(aGroupId)
		, color(aColor)
		, good(aGood)
	{
		brush.CreateSolidBrush(color);
	};

	ItemInfo(const ItemInfo & info)
		: description(info.description)
		, code(info.code)
		, id(info.id)
		, groupId(info.groupId)
		, color(info.color)
		, good(info.good)
	{
		brush.CreateSolidBrush(color);
	};

	ItemInfo & operator =(const ItemInfo & info)
	{
		description = info.description;
		code = info.code;
		id = info.id;
		groupId = info.groupId;
		color = info.color;
		good = info.good;

		brush.DeleteObject();
		brush.CreateSolidBrush(color);

		return *this;
	}

	~ItemInfo()
	{
		brush.DeleteObject();
	}

	void set(const ItemInfo & info)
	{
		description = info.description;
		code = info.code;
		color = info.color;
		brush.DeleteObject();
		brush.CreateSolidBrush(color);
	}

	std::string description;
	std::string code;
	int id;
	int groupId;
	CBrush brush;
	COLORREF color;
	bool good;
};

class DefectContainer
{
public:
	void addGroup(const std::string & name, int id);

	void addDefect(const std::string & description, const std::string & code, int id, int groupId, bool good = false);
	void addDefect(const std::string & description, const std::string & code, int id, int groupId, COLORREF color, bool good = false);

	void setDefect(const ItemInfo & info);
	const std::vector < ItemInfo > & getDefect() const;

	GroupInfo getGroup(int id) const;
	const std::vector < GroupInfo > & getGroup() const;

	std::string getCode(int id) const;
	int getCode(std::string) const;				//Girish
	int getErrorDefectCode(int inspectionErrorCode) const;

	void load(const std::string & name);
	void save(const std::string & name) const;
protected:
	int getId(int id) const;
	std::vector < ItemInfo >::iterator       getInfo(int id);
	std::vector < ItemInfo >::const_iterator getInfo(int id) const;
	ItemInfo getDefect(int nIndex);

	bool isGood(int id) const;
private:
	std::vector < ItemInfo > _defect;
	std::vector < GroupInfo > _group;
};


class CImageSelectionDlg : public CDialogEx, public DefectContainer
{
	DECLARE_DYNAMIC(CImageSelectionDlg)

public:
	CImageSelectionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageSelectionDlg();

	CApplication* m_pApp;
	//CAppDoc* pDoc[MAX_DOCS];

	int nMaxNoDisplay;
	int nImageSets;
	CFont m_fontList;
	BOOL bCloseLButtonThread;
	CString strToolTipText;
	BOOL bActivatedToolTip;
	BOOL *bFovSelected;
	//CImageHistory m_ImageHistoryBright[50];
	//CImageHistory m_ImageHistoryDark[50];
	//CImageHistory m_ImageHistorySingle[50];
	////CImageBuffer m_StorageBufferZoomed[30];
	//CImageHistoryZoom m_ImageHistoryZoomBright[50];
	//CImageHistoryZoom m_ImageHistoryZoomDark[50];
	//CImageHistoryZoom m_ImageHistoryZoomSingle[50];

	CPictureCtrl* m_ctrlDieImages;
	CRgn* m_prgnDieMapPos;
	CRgn* m_prgnDieBounday;

	CRect *m_diePicCtrl;
	BOOL* m_bDieMapPosSel;

	CBrush m_brushUncheck;
	CBrush m_brushBorder;
	CBrush m_brushBorderSelected;
	CBrush m_brushDeviceBorder;
	CScrollBar scrollBarH;
	CScrollBar scrollBarV;
	/*CImageBuffer m_bufBrightImages[50];
	CImageBuffer m_bufDarkImages[50];
	CImageBuffer m_bufSingleSpotImages[50];*/
	
	int m_nDisplayImgSet;
	int nPosNew;
	int nPosOld;

	HANDLE m_hLButtonDownThread;///
	HANDLE m_hPicCtrlImgHistory;///
	CPoint ptLButtonDownPoint;
	CBrush m_pBrushTemp[3];
	CBrush* m_pBrush;
	BOOL bColorFailPass[MAX_DIES];
	BOOL bColorVerifyFailPass[MAX_DIES];
	CPoint ptInRect;
	//CToolTipCtrl m_ToolTip;
// Dialog Data
	enum { IDD = IDD_IMAGESELECTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void Initialize();
	void SetSize(int cx,int cy);
	void DisplayImages(int nTrack);
	void CopyImagesToStats(CImageBuffer** pBufs);
	void ImgCopyToStorageBufZoom(int nTrack, int nImgIndexCount,CImageBuffer* bufTemp);
	void EnableDieRect(int nPos, int nDevice);
	void ClearData();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void InitiDieControls();
	int NavigateDieImages(int nKey);
	void EnableVerification(BOOL bEnble);
	afx_msg void OnBnClickedButtonResultConfirm();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL m_bCheckPass[5];
	int m_nKeyInErrorCode[5];
	CString m_strKeyInErrorCode[5];
	afx_msg void OnBnClickedCheckPart1();
	afx_msg void OnBnClickedCheckPart2();
	afx_msg void OnBnClickedCheckPart3();
	afx_msg void OnBnClickedCheckPart4();
	afx_msg void OnBnClickedCheckPart5();
	//afx_msg BOOL OnToolTipNotify(UINT id,NMHDR* pNMHDR,LRESULT* pResult);
	//afx_msg void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	CString GetDefectName(int nError);
	int GetDefectID(int nErrorCode);
	ItemInfo GetDefectCode(int nIndex);
	afx_msg void OnBnClickedButtonRescan();
};


UINT LButtonDownThread(LPVOID pParam);


class CDialogPane : public CDockablePane
{
	DECLARE_DYNAMIC(CDialogPane)
	DECLARE_MESSAGE_MAP()
public:
	CDialogPane();
	BOOL bIsClosed;
	BOOL bHideEnable;
	virtual ~CDialogPane();
	CImageSelectionDlg m_wndDlg ;

	virtual void OnPressCloseButton();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lp);
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};