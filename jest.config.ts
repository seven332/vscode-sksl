export default {
    roots: [''],
    transform: { '\\.tsx?$': 'ts-jest' },
    testMatch: ['**/packages/*/src/**/*.test.ts'],
    testPathIgnorePatterns: ['/node_modules/', '/integration-test/', '/c\\+\\+/'],
}
