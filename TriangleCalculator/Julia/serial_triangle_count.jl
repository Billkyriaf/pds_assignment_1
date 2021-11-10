using SparseMatricesCSR
using MatrixMarket
using Profile

mutable struct CSR
    A::Array{Int32}
    JA::Array{Int32}
    IA::Array{Int32}

    nonzeros::Int32
    size::Int32
end

function colRowProduct(elements::Array{Int32}, colStart::Int32, colEnd::Int32, rowStart::Int32, rowEnd::Int32)
    res::Int32 = 0
    offset::Int32 = 0
    i::Int32 = 0

    if colEnd - colStart > rowEnd - rowStart
        while i <= (rowEnd - rowStart)
            if offset > colEnd - colStart
                break
            end

            if elements[rowStart + i] < elements[colStart + offset]
                i += 1
            elseif elements[rowStart + i] > elements[colStart + offset]
                offset += 1
            else
                res += 1
                offset += 1
                i += 1
            end
        end
    else
        while i <= (colEnd - colStart)
            if offset > rowEnd - rowStart
                break
            end

            if elements[rowStart + offset] < elements[colStart + i]
                offset += 1
            elseif elements[rowStart + offset] > elements[colStart + i]
                i += 1
            else
                res += 1
                offset += 1
                i += 1
            end
        end
    end

    return res
end


function product(input::CSR)
    output = CSR(Array{Int32}(undef, input.nonzeros),
     Array{Int32}(undef, input.nonzeros),
      Array{Int32}(undef, input.size + 1),
       input.nonzeros,
        input.size)

    nnzInRow::Int32 = 0
    firstRow::Int32 = 0
    lastRow::Int32 = 0

    nnzInCol::Int32 = 0
    firstCol::Int32 = 0
    lastCol::Int32 = 0
    counter::Int32 = 1

    for row = 1:input.size
        nnzInRow = input.IA[row + 1] - input.IA[row]
        firstRow = input.IA[row] + 1
        lastRow = input.IA[row] + nnzInRow

        for colNumber = 1:nnzInRow
            nnzInCol = input.IA[input.JA[firstRow + colNumber - 1] + 1] - input.IA[input.JA[firstRow + colNumber - 1]];
            firstCol = input.IA[input.JA[firstRow + colNumber - 1]] + 1

            lastCol = input.IA[input.JA[firstRow + colNumber - 1]] + nnzInCol

            output.A[counter] = colRowProduct(input.JA, firstCol, lastCol, firstRow, lastRow)
            output.JA[counter] = input.JA[firstRow + colNumber - 1]
            output.IA[row + 1] += 1
            counter += 1
        end
    end

    for i = 2:(output.size + 1)
        output.IA[i] = output.IA[i] + output.IA[i - 1]
    end

    return output
end

function measureTriangles(input::CSR)
    triangles = 0
    for i = 1:input.nonzeros
        triangles += input.A[i]
    end

    return triangles / 6
end


function main()
    stream = open("../../Graphs/belgium_osm/belgium_osm.csr", "r")

    # Readt the title: ##CSR File format
    title = readline(stream, keep = false)

    if title == "##CSR File format"
        # Read the file header: size nonzeros
        header = [parse(Int32, ss) for ss in split(readline(stream, keep = false))]

        csrMatrix = CSR(
        ones(Int32, header[2]),
         [parse(Int32, ss) for ss in split(readline(stream, keep = false))],
          [parse(Int32, ss) for ss in split(readline(stream, keep = false))],
           header[2],
            header[1])

        @profile product(csrMatrix)
        Profile.print(format=:flat)

#         println()
#         println(measureTriangles(v))
    end
end

main()