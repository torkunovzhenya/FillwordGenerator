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
    Node(Node *left, Node *right, Node *up, Node *down, int row, int col);
};


class LinkedMatrix
{
public:

    Node* ChooseCell();

    void AddLocation(std::vector<int> cells);

    std::vector<Node*> ChooseRow(int row);

    void HideRow(Node* row);

    void RestoreRows(std::vector<Node*> rows_hidden);

    void ShowRow(Node* row);

    bool isColumnEmpty(Node* col);

    std::vector<int> getFigureCells(int row);

    LinkedMatrix(int n, int m);

    ~LinkedMatrix();

private:

    int rows;
    int cols;
    std::vector<int> figures_count;
    std::vector<Node*> row_heads;
    std::vector<Node*> col_heads;
    Node* root;
};


#endif //FILLWORDSERVER_LINKEDMATRIX_HPP
