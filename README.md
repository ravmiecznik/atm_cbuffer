# Circular buffer
## Usage can be found in test/test.cpp 
<b><b>

## Install _libcppunit_ library on Ubuntu
```bash
sudo apt-get install libcppunit-doc libcppunit-dev
```

## Run unit tests:
```bash
cd ./test
make && ./test
```

## Example output

```bash
$ make && ./test 
make: 'test' is up to date.
TestCbuffer::test_cbuff_init : OK
TestCbuffer::test_cbuff_put : OK
TestCbuffer::test_cbuff_put_more_than_size : OK
TestCbuffer::test_cbuff_get_more_than_size : OK
TestCbuffer::test_fill_buffer_with_data_fit_size : OK
OK (5)
```
