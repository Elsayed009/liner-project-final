#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

int     equationCount;
int     maxVars;         // determined at runtime after reading input
float*  rightHandSide;
string* varNames;
float*  varCoeffs;
int*    varCount;

int slot(int eqIndex, int varSlot) {
    return eqIndex * maxVars + varSlot;
}

int findVar(int eqIndex, const string& name) {
    for (int j = 0; j < varCount[eqIndex]; j++)
        if (varNames[slot(eqIndex, j)] == name) return j;
    return -1;
}

void addToCoeff(int eqIndex, const string& name, float value) {
    int index = findVar(eqIndex, name);
    if (index == -1) {
        int s = slot(eqIndex, varCount[eqIndex]);
        varNames[s]  = name;
        varCoeffs[s] = value;
        varCount[eqIndex]++;
    } else {
        varCoeffs[slot(eqIndex, index)] += value;
    }
}

float getCoeff(int eqIndex, const string& name) {
    int index = findVar(eqIndex, name);
    return (index == -1) ? 0 : varCoeffs[slot(eqIndex, index)];
}

void sortVars(int eqIndex) {
    int count = varCount[eqIndex];
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - i - 1; j++) {
            int a = slot(eqIndex, j), b = slot(eqIndex, j+1);
            if (varNames[a] > varNames[b]) {
                swap(varNames[a],  varNames[b]);
                swap(varCoeffs[a], varCoeffs[b]);
            }
        }
}

string equationToString(int eqIndex) {
    sortVars(eqIndex);
    ostringstream output;
    bool isFirstTerm = true;
    for (int j = 0; j < varCount[eqIndex]; j++) {
        float coeff = varCoeffs[slot(eqIndex, j)];
        if (coeff == 0) continue;
        if (!isFirstTerm && coeff > 0) output << "+";
        if (coeff == (int)coeff) output << (int)coeff;
        else                     output << coeff;
        output << varNames[slot(eqIndex, j)];
        isFirstTerm = false;
    }
    if (isFirstTerm) output << "0";
    output << "=";
    if (rightHandSide[eqIndex] == (int)rightHandSide[eqIndex]) output << (int)rightHandSide[eqIndex];
    else                                                         output << rightHandSide[eqIndex];
    return output.str();
}

// prints a local result stored in temporary arrays
string resultToString(string* resNames, float* resCoeffs, int resCount, float resRHS) {
    // sort first
    for (int a = 0; a < resCount - 1; a++)
        for (int b = 0; b < resCount - a - 1; b++)
            if (resNames[b] > resNames[b+1]) { swap(resNames[b], resNames[b+1]); swap(resCoeffs[b], resCoeffs[b+1]); }
    ostringstream out;
    bool first = true;
    for (int k = 0; k < resCount; k++) {
        if (resCoeffs[k] == 0) continue;
        if (!first && resCoeffs[k] > 0) out << "+";
        if (resCoeffs[k] == (int)resCoeffs[k]) out << (int)resCoeffs[k];
        else out << resCoeffs[k];
        out << resNames[k];
        first = false;
    }
    if (first) out << "0";
    out << "=";
    if (resRHS == (int)resRHS) out << (int)resRHS;
    else out << resRHS;
    return out.str();
}

void parseLeftSide(const string& leftSide, int eqIndex) {
    string input = leftSide;
    if (input.empty()) return;
    if (input[0] != '-') input = "+" + input;

    // allocate terms on the heap
    string* terms     = new string[input.size()];
    int     termCount = 0;
    string  currentTerm = "";

    for (int i = 0; i < (int)input.size(); i++) {
        if ((input[i] == '+' || input[i] == '-') && i != 0) {
            if (!currentTerm.empty()) terms[termCount++] = currentTerm;
            currentTerm = input[i];
        } else currentTerm += input[i];
    }
    if (!currentTerm.empty()) terms[termCount++] = currentTerm;

    for (int t = 0; t < termCount; t++) {
        string& term = terms[t];
        int letterPos = -1;
        for (int i = 0; i < (int)term.size(); i++)
            if (term[i] >= 'a' && term[i] <= 'z') { letterPos = i; break; }
        if (letterPos == -1) {
            rightHandSide[eqIndex] -= atof(term.c_str());
        } else {
            string coeffPart = term.substr(0, letterPos);
            string varName   = term.substr(letterPos);
            float  coeff;
            if (coeffPart.empty() || coeffPart == "+") coeff = 1;
            else if (coeffPart == "-")                  coeff = -1;
            else                                        coeff = atof(coeffPart.c_str());
            addToCoeff(eqIndex, varName, coeff);
        }
    }
    delete[] terms;
}

int collectAllVars(string*& outVars) {
    // worst case: every equation has maxVars unique variables
    string* uniqueVars  = new string[equationCount * maxVars];
    int     uniqueCount = 0;
    for (int i = 0; i < equationCount; i++)
        for (int j = 0; j < varCount[i]; j++) {
            string name = varNames[slot(i, j)];
            bool found  = false;
            for (int k = 0; k < uniqueCount; k++)
                if (uniqueVars[k] == name) { found = true; break; }
            if (!found) uniqueVars[uniqueCount++] = name;
        }
    for (int i = 0; i < uniqueCount - 1; i++)
        for (int j = 0; j < uniqueCount - i - 1; j++)
            if (uniqueVars[j] > uniqueVars[j+1]) swap(uniqueVars[j], uniqueVars[j+1]);
    outVars = uniqueVars;
    return uniqueCount;
}

float calcDeterminant(float** matrix, int size) {
    if (size == 1) return matrix[0][0];
    if (size == 2) return matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[1][0];
    float   result    = 0;
    float** subMatrix = new float*[size-1];
    for (int i = 0; i < size-1; i++) subMatrix[i] = new float[size-1];
    for (int col = 0; col < size; col++) {
        for (int row = 1; row < size; row++) {
            int subCol = 0;
            for (int c = 0; c < size; c++)
                if (c != col) subMatrix[row-1][subCol++] = matrix[row][c];
        }
        result += matrix[0][col] * calcDeterminant(subMatrix, size-1) * (col%2==0 ? 1 : -1);
    }
    for (int i = 0; i < size-1; i++) delete[] subMatrix[i];
    delete[] subMatrix;
    return result;
}

float** buildCoeffMatrix(string* allVars, int size, string replaceVar = "") {
    float** matrix = new float*[equationCount];
    for (int row = 0; row < equationCount; row++) {
        matrix[row] = new float[size];
        for (int col = 0; col < size; col++)
            matrix[row][col] = (!replaceVar.empty() && allVars[col] == replaceVar)
                               ? rightHandSide[row]
                               : getCoeff(row, allVars[col]);
    }
    return matrix;
}

void freeMatrix(float** matrix) {
    for (int i = 0; i < equationCount; i++) delete[] matrix[i];
    delete[] matrix;
}

void printNumber(float value) {
    if (value == (int)value) cout << (int)value;
    else                     cout << value;
}

int main() {
    cin >> equationCount;

    // read all lines first so we can count max variables per equation
    string* lines = new string[equationCount];
    for (int i = 0; i < equationCount; i++) {
        cin >> ws;
        getline(cin, lines[i]);
    }

    // estimate maxVars: count '+' and '-' signs in each line as a safe upper bound
    maxVars = 0;
    for (int i = 0; i < equationCount; i++) {
        int count = 1;
        for (char c : lines[i]) if (c == '+' || c == '-') count++;
        if (count > maxVars) maxVars = count;
    }

    // now allocate all heap arrays with the right sizes
    rightHandSide = new float[equationCount];
    varCount      = new int[equationCount];
    varNames      = new string[equationCount * maxVars];
    varCoeffs     = new float[equationCount * maxVars];

    for (int i = 0; i < equationCount; i++) {
        varCount[i]      = 0;
        rightHandSide[i] = 0;
    }

    // parse each equation
    for (int i = 0; i < equationCount; i++) {
        size_t equalSign   = lines[i].find('=');
        rightHandSide[i]   = atof(lines[i].substr(equalSign+1).c_str());
        parseLeftSide(lines[i].substr(0, equalSign), i);
        sortVars(i);
    }
    delete[] lines;

    string command;
    while (cin >> command) {
        if (command == "quit") break;

        else if (command == "num_vars") {
            string* varList = nullptr;
            cout << collectAllVars(varList) << endl;
            delete[] varList;
        }

        else if (command == "equation") {
            int index; cin >> index;
            cout << equationToString(index-1) << endl;
        }

        else if (command == "column") {
            string name; cin >> name;
            for (int i = 0; i < equationCount; i++) {
                printNumber(getCoeff(i, name));
                cout << endl;
            }
        }

        else if (command == "add") {
            int i, j; cin >> i >> j;
            string* resNames  = new string[maxVars * 2];
            float*  resCoeffs = new float[maxVars * 2];
            int     resCount  = varCount[i-1];
            float   resRHS    = rightHandSide[i-1] + rightHandSide[j-1];
            for (int k = 0; k < resCount; k++) {
                resNames[k]  = varNames[slot(i-1, k)];
                resCoeffs[k] = varCoeffs[slot(i-1, k)];
            }
            for (int k = 0; k < varCount[j-1]; k++) {
                string name = varNames[slot(j-1, k)];
                float  val  = varCoeffs[slot(j-1, k)];
                bool found  = false;
                for (int x = 0; x < resCount; x++)
                    if (resNames[x] == name) { resCoeffs[x] += val; found = true; break; }
                if (!found) { resNames[resCount] = name; resCoeffs[resCount++] = val; }
            }
            cout << resultToString(resNames, resCoeffs, resCount, resRHS) << endl;
            delete[] resNames; delete[] resCoeffs;
        }

        else if (command == "subtract") {
            int i, j; cin >> i >> j;
            string* resNames  = new string[maxVars * 2];
            float*  resCoeffs = new float[maxVars * 2];
            int     resCount  = varCount[i-1];
            float   resRHS    = rightHandSide[i-1] - rightHandSide[j-1];
            for (int k = 0; k < resCount; k++) {
                resNames[k]  = varNames[slot(i-1, k)];
                resCoeffs[k] = varCoeffs[slot(i-1, k)];
            }
            for (int k = 0; k < varCount[j-1]; k++) {
                string name = varNames[slot(j-1, k)];
                float  val  = -varCoeffs[slot(j-1, k)];
                bool found  = false;
                for (int x = 0; x < resCount; x++)
                    if (resNames[x] == name) { resCoeffs[x] += val; found = true; break; }
                if (!found) { resNames[resCount] = name; resCoeffs[resCount++] = val; }
            }
            cout << resultToString(resNames, resCoeffs, resCount, resRHS) << endl;
            delete[] resNames; delete[] resCoeffs;
        }

        else if (command == "substitute") {
            string targetVar; int eqIndex, subIndex;
            cin >> targetVar >> eqIndex >> subIndex;
            float coeffInSource = getCoeff(eqIndex-1, targetVar);
            float coeffInSub    = getCoeff(subIndex-1, targetVar);
            if (coeffInSource == 0 || coeffInSub == 0) { cout << equationToString(eqIndex-1) << endl; continue; }
            float   scaleFactor = coeffInSource / coeffInSub;
            string* resNames    = new string[maxVars * 2];
            float*  resCoeffs   = new float[maxVars * 2];
            int     resCount    = varCount[eqIndex-1];
            float   resRHS      = rightHandSide[eqIndex-1] - scaleFactor * rightHandSide[subIndex-1];
            for (int k = 0; k < resCount; k++) {
                resNames[k]  = varNames[slot(eqIndex-1, k)];
                resCoeffs[k] = varCoeffs[slot(eqIndex-1, k)];
            }
            for (int k = 0; k < varCount[subIndex-1]; k++) {
                string name = varNames[slot(subIndex-1, k)];
                float  val  = -scaleFactor * varCoeffs[slot(subIndex-1, k)];
                bool found  = false;
                for (int x = 0; x < resCount; x++)
                    if (resNames[x] == name) { resCoeffs[x] += val; found = true; break; }
                if (!found) { resNames[resCount] = name; resCoeffs[resCount++] = val; }
            }
            // force target variable to exactly 0
            for (int k = 0; k < resCount; k++)
                if (resNames[k] == targetVar) resCoeffs[k] = 0;
            cout << resultToString(resNames, resCoeffs, resCount, resRHS) << endl;
            delete[] resNames; delete[] resCoeffs;
        }

        else if (command == "D") {
            string rest, replaceVar = "";
            getline(cin, rest);
            stringstream ss(rest);
            ss >> replaceVar;
            string* varList = nullptr;
            int size = collectAllVars(varList);
            float** matrix = buildCoeffMatrix(varList, size, replaceVar);
            for (int row = 0; row < equationCount; row++) {
                for (int col = 0; col < size; col++) {
                    printNumber(matrix[row][col]);
                    if (col < size-1) cout << "\t";
                }
                cout << endl;
            }
            freeMatrix(matrix);
            delete[] varList;
        }

        else if (command == "D_value") {
            string* varList = nullptr;
            int size = collectAllVars(varList);
            float** matrix = buildCoeffMatrix(varList, size);
            printNumber(calcDeterminant(matrix, size));
            cout << endl;
            freeMatrix(matrix);
            delete[] varList;
        }

        else if (command == "solve") {
            string* varList = nullptr;
            int size = collectAllVars(varList);
            float** matrix  = buildCoeffMatrix(varList, size);
            float   mainDet = calcDeterminant(matrix, size);
            if (mainDet == 0) {
                cout << "No Solution" << endl;
            } else {
                for (int k = 0; k < size; k++) {
                    float** cramersMatrix = buildCoeffMatrix(varList, size, varList[k]);
                    cout << varList[k] << "=";
                    printNumber(calcDeterminant(cramersMatrix, size) / mainDet);
                    cout << endl;
                    freeMatrix(cramersMatrix);
                }
            }
            freeMatrix(matrix);
            delete[] varList;
        }
    }

    delete[] rightHandSide;
    delete[] varCount;
    delete[] varNames;
    delete[] varCoeffs;
    return 0;
}