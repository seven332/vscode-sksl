export function findFirstGE<E, V>(array: E[], value: V, comp: (element: E, value: V) => boolean): number {
    let len = array.length
    let first = 0
    while (len != 0) {
        const l2 = (len / 2) >> 0
        const m = first + l2
        if (comp(array[m], value)) {
            first = m + 1
            len -= l2 + 1
        } else {
            len = l2
        }
    }
    return first >= array.length ? -1 : first
}

export function findFirstGT<E, V>(array: E[], value: V, comp: (value: V, element: E) => boolean): number {
    let len = array.length
    let first = 0
    while (len != 0) {
        const l2 = (len / 2) >> 0
        const m = first + l2
        if (comp(value, array[m])) {
            len = l2
        } else {
            first = m + 1
            len -= l2 + 1
        }
    }
    return first >= array.length ? -1 : first
}

export function findLastLE<E, V>(array: E[], value: V, comp: (value: V, element: E) => boolean): number {
    const index = findFirstGT(array, value, comp)
    if (index == -1) {
        // all elements <= value
        // return the last index
        return array.length - 1
    } else {
        // return the index before first >
        return index - 1
    }
}

export function findLastLT<E, V>(array: E[], value: V, comp: (element: E, value: V) => boolean): number {
    const index = findFirstGE(array, value, comp)
    if (index == -1) {
        // all elements < value
        // return the last index
        return array.length - 1
    } else {
        // return the index before first >=
        return index - 1
    }
}
