SetTarget(
    MODE STATIC
    TARGET_NAME Homework2
    DEFINE
        PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/"
        NOMINMAX
    INC
        ${PROJECT_SOURCE_DIR}/Base/ANN/include
    LIB
        Base
)