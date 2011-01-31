struct AB {
    int a;
    int b;
};

void f(struct AB *ab) {
    if (ab || ab->a == 0) {    // null-pointer dereference if ab is NULL

    }
}


