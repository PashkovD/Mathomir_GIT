#pragma once


class CExpression;

class CElement
{
public:
    void* operator new(size_t size);
    //void operator delete(void *p);

public:
    CExpression* m_pPaternalExpression;
    short* Data3;
    CExpression* Expression1;
    CExpression* Expression2;
    CExpression* Expression3;
    short ParenthesesBelow;
    short ParenthesesAbove;

    byte m_Type; //type of Element
    byte m_Color;
    byte m_Text;
    unsigned char m_VMods; //variable modificators (overhead dash, hat, arrow, dot, double dot, measurement unit)

    //the below codes must have fixed structure (see the 'operator new' and 'CopyElement')
    short E1_length, E1_above, E1_below, E1_posX, E1_posY;
    char Data2[10];
    short E2_length, E2_above, E2_below, E2_posX, E2_posY;
    short E3_length, E3_above, E3_below, E3_posX, E3_posY;
    char Data1[10];

public:
    CElement();
    ~CElement();
    void CalculateSize(CDC& DC, short int zoom, short int& length, short int& above, short int& below,
                       short paternal_position, char HQR);
    void PaintExpression(CDC* DC, short zoom, short X, short Y, bool IsBlue, int ActualSize, RECT* ClipReg = nullptr,
                         COLORREF color = 0);
    void Empty(char oper);
    void CopyElement(const CElement* Element);
    int FontSizeForType(int subelement_no) const;

    CObject* SelectAtPoint(CDC* DC, short zoom, short X, short Y, short* IsExpression, char* IsParenthese,
                           short paternal_position);
    int XML_output(char* output, int num_tabs, char only_calculate);
    int LaTeX_output(char* output, char only_calculate) const;
    char* XML_input(char* file, void* element_struct);
    int CalcChecksum() const;

    int IsMeasurementUnit() const;
    int SetColor(int color);
    int IsDifferential(int only_nonparentheses = 0) const;

    CElement* GetPreviousElement() const;
    CElement* GetNextElement() const;
    int GetPaternalPosition() const;
    int ContainsBlinkingCursor() const;
    void CalculateSizeReadjust(short zoom, short* length, short* above, short* below, char align);
};
