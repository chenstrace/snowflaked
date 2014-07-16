Snowflake PHP Client

This extension provides an API for communicating with Snowflaked.

# Building

    phpize
    ./configure
    make
    sudo make install

Once built and installed, be sure to add 'snowflake.so' to the php.ini extensions configuration.

    extension=barbershop.so

# Use

This extension exposes one class, 'Snowflake'.

    $bs = new Barbershop();
    $bs->connect('127.0.0.1', 8008);
    assert($bs->update('5000', 1) == "+OK");
    assert($bs->peek() == "+5000");
    assert($bs->next() == "+5000");
    assert($bs->next() == "+-1");

# TODO

 * Create real class documentation.
 * Change the class method responses.