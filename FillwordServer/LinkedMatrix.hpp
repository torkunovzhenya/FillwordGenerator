#ifndef FILLWORDSERVER_LINKEDMATRIX_HPP
#define FILLWORDSERVER_LINKEDMATRIX_HPP

#include <vector>

class Node{
public:

    Node* left = nullptr;
    Node* right = nullptr;
    Node* up = nullptr;
    Node* down = nullptr;
    int row;
    int col;

    Node(int row, int col);
};


class LinkedMatrix
{
public:

    LinkedMatrix(int n, int m);

    void AddLocation(std::vector<int> cells);

    Node* ChooseCell();

    std::vector<Node*> ChooseRow(int row);

    void RestoreRows(std::vector<Node*> rows_hidden);

    bool isColumnEmpty(Node* col);

    std::vector<int> getFigureCells(int row);

    ~LinkedMatrix();

private:

    int rows;
    int cols;
    std::vector<int> figures_count;
    std::vector<Node*> row_heads;
    std::vector<Node*> col_heads;
    Node* root;

    void HideRow(Node* row);

    void ShowRow(Node* row);
};


#endif //FILLWORDSERVER_LINKEDMATRIX_HPP
